#include "../include/TechnicalIndicator.h"
#include "../include/Scheduler.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <vector>
#include <random>
#include <algorithm>
#include <thread>
#ifdef _OPENMP
#include <omp.h>
#endif

class PerformanceMonitor {
public:
    void start() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void stop() {
        endTime_ = std::chrono::high_resolution_clock::now();
    }
    
    double getElapsedSeconds() const {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime_ - startTime_);
        return duration.count() / 1000.0;
    }
    
    double getElapsedMilliseconds() const {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime_ - startTime_);
        return duration.count();
    }

private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point endTime_;
};

std::vector<TechnicalIndicator::StockData> generateSampleData(int numStocks) {
    std::vector<TechnicalIndicator::StockData> stocks;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> priceDist(100.0, 200.0);
    std::uniform_real_distribution<double> volumeDist(1000000.0, 10000000.0);
    
    std::vector<std::string> symbols = {
        "AAPL", "GOOGL", "MSFT", "AMZN", "TSLA", "META", "NVDA", "JPM", 
        "V", "JNJ", "WMT", "PG", "MA", "UNH", "HD", "DIS", "BAC", "XOM",
        "CVX", "ABBV", "PFE", "KO", "AVGO", "COST", "MRK", "PEP", "TMO",
        "CSCO", "ABT", "ACN", "NFLX", "ADBE", "CMCSA", "NKE", "TXN", "DHR"
    };
    
    for (int i = 0; i < numStocks && i < static_cast<int>(symbols.size()); ++i) {
        TechnicalIndicator::StockData stock;
        stock.symbol = symbols[i];
        
        double basePrice = priceDist(gen);
        for (int day = 0; day < 100; ++day) {
            basePrice += (priceDist(gen) - basePrice) * 0.1;
            stock.prices.push_back(basePrice);
            stock.volumes.push_back(volumeDist(gen));
            stock.timestamps.push_back(day);
        }
        
        stocks.push_back(stock);
    }
    
    while (static_cast<int>(stocks.size()) < numStocks) {
        TechnicalIndicator::StockData stock = stocks[stocks.size() % symbols.size()];
        stock.symbol = "STOCK" + std::to_string(stocks.size());
        stocks.push_back(stock);
    }
    
    return stocks;
}

std::vector<TechnicalIndicator::IndicatorResult> computeSequential(
    const std::vector<TechnicalIndicator::StockData>& stocks) {
    
    TechnicalIndicator indicator;
    std::vector<TechnicalIndicator::IndicatorResult> results;
    
    for (const auto& stock : stocks) {
        results.push_back(indicator.computeIndicators(stock));
    }
    
    return results;
}

std::vector<TechnicalIndicator::IndicatorResult> computeParallel(
    const std::vector<TechnicalIndicator::StockData>& stocks) {
    
    TechnicalIndicator indicator;
    return indicator.computeIndicatorsParallel(stocks);
}

void printResults(const std::vector<TechnicalIndicator::IndicatorResult>& results) {
    std::cout << "\n=== Analysis Results ===\n";
    std::cout << std::left << std::setw(10) << "Symbol"
              << std::setw(10) << "Signal"
              << std::setw(12) << "Strength"
              << std::setw(10) << "RSI"
              << std::setw(10) << "SMA20"
              << std::setw(10) << "SMA50"
              << "\n";
    std::cout << std::string(72, '-') << "\n";
    
    for (const auto& result : results) {
        std::cout << std::left << std::setw(10) << result.symbol
                  << std::setw(10) << result.signal
                  << std::setw(12) << std::fixed << std::setprecision(2) << result.signal_strength
                  << std::setw(10) << std::fixed << std::setprecision(2) << result.rsi
                  << std::setw(10) << std::fixed << std::setprecision(2) << result.sma_20
                  << std::setw(10) << std::fixed << std::setprecision(2) << result.sma_50
                  << "\n";
    }
}

int main(int argc, char* argv[]) {
    std::cout << "=== Parallel Stock Market Analysis System ===\n\n";
    
    int numThreads = 1;
    #ifdef _OPENMP
    numThreads = omp_get_max_threads();
    std::cout << "OpenMP enabled with " << numThreads << " threads\n";
    #else
    std::cout << "OpenMP not available - running sequentially\n";
    #endif
    
    int numStocks = 10;
    bool runScheduler = false;
    bool benchmark = true;
    
    if (argc > 1) {
        numStocks = std::stoi(argv[1]);
    }
    if (argc > 2) {
        runScheduler = (std::string(argv[2]) == "scheduler");
    }
    if (argc > 3) {
        benchmark = (std::string(argv[3]) != "no-benchmark");
    }
    
    std::cout << "Number of stocks: " << numStocks << "\n";
    std::cout << "Run scheduler: " << (runScheduler ? "Yes" : "No") << "\n\n";
    
    std::cout << "Generating sample stock data...\n";
    auto stocks = generateSampleData(numStocks);
    std::cout << "Generated data for " << stocks.size() << " stocks\n\n";
    
    if (benchmark) {
        std::cout << "=== Performance Benchmark ===\n";
        
        PerformanceMonitor monitor;
        
        std::cout << "Running sequential computation...\n";
        monitor.start();
        auto sequentialResults = computeSequential(stocks);
        monitor.stop();
        double sequentialTime = monitor.getElapsedSeconds();
        std::cout << "Sequential time: " << sequentialTime << " seconds\n";
        
        std::cout << "Running parallel computation...\n";
        monitor.start();
        auto parallelResults = computeParallel(stocks);
        monitor.stop();
        double parallelTime = monitor.getElapsedSeconds();
        std::cout << "Parallel time: " << parallelTime << " seconds\n";
        
        double speedup = sequentialTime / parallelTime;
        double efficiency = speedup / numThreads;
        
        std::cout << "\n=== Performance Summary ===\n";
        std::cout << "Sequential time: " << std::fixed << std::setprecision(3) 
                  << sequentialTime << " seconds\n";
        std::cout << "Parallel time: " << std::fixed << std::setprecision(3) 
                  << parallelTime << " seconds\n";
        std::cout << "Speedup: " << std::fixed << std::setprecision(2) 
                  << speedup << "x\n";
        std::cout << "Efficiency: " << std::fixed << std::setprecision(2) 
                  << (efficiency * 100) << "%\n";
        
        bool resultsMatch = true;
        if (sequentialResults.size() == parallelResults.size()) {
            for (size_t i = 0; i < sequentialResults.size(); ++i) {
                if (sequentialResults[i].symbol != parallelResults[i].symbol ||
                    std::abs(sequentialResults[i].rsi - parallelResults[i].rsi) > 0.01) {
                    resultsMatch = false;
                    break;
                }
            }
        } else {
            resultsMatch = false;
        }
        
        std::cout << "Results match: " << (resultsMatch ? "Yes" : "No") << "\n\n";
        
        std::cout << "Sample results (first 10):\n";
        std::vector<TechnicalIndicator::IndicatorResult> sampleResults(
            parallelResults.begin(),
            parallelResults.begin() + std::min(10, static_cast<int>(parallelResults.size()))
        );
        printResults(sampleResults);
    }
    
    if (runScheduler) {
        std::cout << "\n=== Starting Scheduler Mode ===\n";
        
        TechnicalIndicator indicator;
        Scheduler scheduler(10);
        
        scheduler.setAnalysisCallback([&indicator, &scheduler](
            const std::vector<TechnicalIndicator::StockData>& stocks) {
            
            std::cout << "\n[Scheduler] Running analysis on " << stocks.size() << " stocks\n";
            
            PerformanceMonitor monitor;
            monitor.start();
            auto results = indicator.computeIndicatorsParallel(stocks);
            monitor.stop();
            
            std::cout << "[Scheduler] Analysis completed in " 
                      << monitor.getElapsedMilliseconds() << " ms\n";
            
            for (const auto& result : results) {
                scheduler.getNotificationQueue().push(result);
            }
        });
        
        // Set up notification callback
        scheduler.setNotificationCallback([](const TechnicalIndicator::IndicatorResult& result) {
            std::cout << "[Notification] " << result.signal << " signal for " 
                      << result.symbol << " (Strength: " << result.signal_strength << ")\n";
        });
        
        for (const auto& stock : stocks) {
            scheduler.addStockData(stock);
        }
        
        scheduler.start();
        
        std::cout << "Running scheduler for 30 seconds...\n";
        std::this_thread::sleep_for(std::chrono::seconds(30));
        
        scheduler.stop();
        std::cout << "\nScheduler stopped\n";
    }
    
    std::cout << "\n=== Program Complete ===\n";
    return 0;
}

