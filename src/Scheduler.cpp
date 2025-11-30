#include "../include/Scheduler.h"
#include <iostream>
#include <chrono>
#include <random>

Scheduler::Scheduler(int intervalSeconds)
    : intervalSeconds_(intervalSeconds), running_(false), shouldStop_(false) {
}

Scheduler::~Scheduler() {
    stop();
}

void Scheduler::start() {
    if (running_) {
        return;
    }

    running_ = true;
    shouldStop_ = false;
    
    // Start scheduler thread
    schedulerThread_ = std::thread(&Scheduler::schedulerThread, this);
    
    // Start data fetcher thread
    dataFetcherThread_ = std::thread(&Scheduler::dataFetcherThread, this);
    
    // Start notification dispatcher thread
    notificationDispatcherThread_ = std::thread(&Scheduler::notificationDispatcherThread, this);
    
    std::cout << "[Scheduler] Started with interval: " << intervalSeconds_ << " seconds\n";
}

void Scheduler::stop() {
    if (!running_) {
        return;
    }

    shouldStop_ = true;
    running_ = false;
    
    // Signal queues to stop
    dataQueue_.stop();
    notificationQueue_.stop();
    
    // Wait for threads to finish
    if (schedulerThread_.joinable()) {
        schedulerThread_.join();
    }
    if (dataFetcherThread_.joinable()) {
        dataFetcherThread_.join();
    }
    if (notificationDispatcherThread_.joinable()) {
        notificationDispatcherThread_.join();
    }
    
    std::cout << "[Scheduler] Stopped\n";
}

void Scheduler::setAnalysisCallback(AnalysisCallback callback) {
    analysisCallback_ = callback;
}

void Scheduler::setNotificationCallback(NotificationCallback callback) {
    notificationCallback_ = callback;
}

void Scheduler::addStockData(const TechnicalIndicator::StockData& stockData) {
    std::lock_guard<std::mutex> lock(cacheMutex_);
    stockDataCache_.push_back(stockData);
}

ThreadSafeQueue<TechnicalIndicator::IndicatorResult>& Scheduler::getNotificationQueue() {
    return notificationQueue_;
}

/**
 * Scheduler thread function - manages periodic analysis cycles.
 * 
 * This thread demonstrates task parallelism by running independently
 * from other threads. It triggers analysis cycles at regular intervals
 * (default: hourly) and coordinates the overall system workflow.
 * 
 * Thread Safety: Uses mutex to safely access shared stockDataCache_
 */
void Scheduler::schedulerThread() {
    std::cout << "[Scheduler] Thread started\n";
    
    while (!shouldStop_) {
        auto startTime = std::chrono::steady_clock::now();
        
        // Trigger analysis cycle
        // Lock mutex to safely access shared stock data cache
        {
            std::lock_guard<std::mutex> lock(cacheMutex_);
            if (!stockDataCache_.empty() && analysisCallback_) {
                std::cout << "[Scheduler] Triggering analysis cycle for " 
                          << stockDataCache_.size() << " stocks\n";
                // Callback will trigger OpenMP parallel computation
                analysisCallback_(stockDataCache_);
            }
        }
        
        // Wait for the interval, checking for stop signal periodically
        // This allows graceful shutdown without waiting for full interval
        auto targetTime = startTime + std::chrono::seconds(intervalSeconds_);
        while (std::chrono::steady_clock::now() < targetTime && !shouldStop_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    std::cout << "[Scheduler] Thread stopped\n";
}

void Scheduler::dataFetcherThread() {
    std::cout << "[DataFetcher] Thread started\n";
    
    // Simulate data fetching - in real implementation, this would fetch from API
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> priceDist(100.0, 200.0);
    std::uniform_real_distribution<double> volumeDist(1000000.0, 10000000.0);
    
    std::vector<std::string> symbols = {"AAPL", "GOOGL", "MSFT", "AMZN", "TSLA", 
                                        "META", "NVDA", "JPM", "V", "JNJ"};
    
    while (!shouldStop_) {
        // Simulate periodic data updates
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        if (shouldStop_) break;
        
        // Generate sample data for each symbol
        for (const auto& symbol : symbols) {
            TechnicalIndicator::StockData stockData;
            stockData.symbol = symbol;
            
            // Generate historical price data (e.g., 100 days)
            for (int i = 0; i < 100; ++i) {
                stockData.prices.push_back(priceDist(gen));
                stockData.volumes.push_back(volumeDist(gen));
                stockData.timestamps.push_back(i);
            }
            
            dataQueue_.push(stockData);
        }
        
        std::cout << "[DataFetcher] Fetched data for " << symbols.size() << " stocks\n";
    }
    
    std::cout << "[DataFetcher] Thread stopped\n";
}

void Scheduler::notificationDispatcherThread() {
    std::cout << "[NotificationDispatcher] Thread started\n";
    
    while (!shouldStop_) {
        // Try to get a notification from the queue
        auto result = notificationQueue_.tryPop();
        
        if (result.has_value()) {
            TechnicalIndicator::IndicatorResult notification = result.value();
            
            // Only dispatch notifications for BUY or SELL signals
            if (notification.signal == "BUY" || notification.signal == "SELL") {
                std::cout << "[NotificationDispatcher] Signal: " << notification.signal 
                          << " for " << notification.symbol 
                          << " (Strength: " << notification.signal_strength << ")\n";
                
                if (notificationCallback_) {
                    notificationCallback_(notification);
                }
            }
        } else {
            // No notifications available, sleep briefly
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    std::cout << "[NotificationDispatcher] Thread stopped\n";
}

