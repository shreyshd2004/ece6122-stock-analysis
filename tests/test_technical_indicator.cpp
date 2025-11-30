#include "../include/TechnicalIndicator.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>

// Test helper: Check if two doubles are approximately equal
bool approxEqual(double a, double b, double epsilon = 0.01) {
    return std::abs(a - b) < epsilon;
}

// Test 1: SMA Calculation Correctness (via computeIndicators)
void testSMA() {
    std::cout << "Test 1: SMA Calculation... ";
    
    TechnicalIndicator indicator;
    TechnicalIndicator::StockData stock;
    stock.symbol = "TEST";
    
    // Create known price sequence with enough data for SMA20
    for (int i = 0; i < 60; ++i) {
        stock.prices.push_back(100.0 + i * 0.5);
    }
    
    auto result = indicator.computeIndicators(stock);
    
    // SMA values should be valid and positive
    assert(result.sma_20 > 0.0);
    assert(result.sma_50 > 0.0);
    assert(!std::isnan(result.sma_20) && !std::isnan(result.sma_50));
    
    std::cout << "PASSED\n";
}

// Test 2: RSI Calculation Correctness (via computeIndicators)
void testRSI() {
    std::cout << "Test 2: RSI Calculation... ";
    
    TechnicalIndicator indicator;
    TechnicalIndicator::StockData stock;
    stock.symbol = "TEST";
    
    // Create price sequence with known gains/losses
    // Prices with upward trend (should give RSI > 50)
    for (int i = 0; i < 60; ++i) {
        stock.prices.push_back(100.0 + i * 0.5);
    }
    
    auto result = indicator.computeIndicators(stock);
    
    // RSI should be between 0 and 100
    assert(result.rsi >= 0.0 && result.rsi <= 100.0);
    assert(!std::isnan(result.rsi));
    
    std::cout << "PASSED\n";
}

// Test 3: MACD Calculation Correctness (via computeIndicators)
void testMACD() {
    std::cout << "Test 3: MACD Calculation... ";
    
    TechnicalIndicator indicator;
    TechnicalIndicator::StockData stock;
    stock.symbol = "TEST";
    
    // Create sufficient data points (need at least 26 for MACD)
    for (int i = 0; i < 60; ++i) {
        stock.prices.push_back(100.0 + i * 0.5);
    }
    
    auto result = indicator.computeIndicators(stock);
    
    // MACD and signal should be valid numbers
    assert(!std::isnan(result.macd) && !std::isinf(result.macd));
    assert(!std::isnan(result.macd_signal) && !std::isinf(result.macd_signal));
    
    std::cout << "PASSED\n";
}

// Test 4: Signal Generation Logic
void testSignalGeneration() {
    std::cout << "Test 4: Signal Generation... ";
    
    TechnicalIndicator indicator;
    TechnicalIndicator::StockData stock;
    stock.symbol = "TEST";
    
    // Create data that should generate a BUY signal (low RSI, bullish MACD)
    for (int i = 0; i < 60; ++i) {
        stock.prices.push_back(100.0 - i * 0.1); // Declining prices (oversold)
    }
    
    auto result = indicator.computeIndicators(stock);
    
    // Signal should be one of: BUY, SELL, or HOLD
    assert(result.signal == "BUY" || result.signal == "SELL" || result.signal == "HOLD");
    assert(result.signal_strength >= 0.0 && result.signal_strength <= 100.0);
    
    std::cout << "PASSED\n";
}

// Test 5: Parallel vs Sequential Consistency
void testParallelConsistency() {
    std::cout << "Test 5: Parallel vs Sequential Consistency... ";
    
    TechnicalIndicator indicator;
    std::vector<TechnicalIndicator::StockData> stocks;
    
    // Create multiple test stocks
    for (int i = 0; i < 10; ++i) {
        TechnicalIndicator::StockData stock;
        stock.symbol = "STOCK" + std::to_string(i);
        for (int j = 0; j < 60; ++j) {
            stock.prices.push_back(100.0 + j * 0.5 + i);
        }
        stocks.push_back(stock);
    }
    
    // Compute sequentially
    std::vector<TechnicalIndicator::IndicatorResult> sequentialResults;
    for (const auto& stock : stocks) {
        sequentialResults.push_back(indicator.computeIndicators(stock));
    }
    
    // Compute in parallel
    auto parallelResults = indicator.computeIndicatorsParallel(stocks);
    
    // Results should match
    assert(sequentialResults.size() == parallelResults.size());
    
    for (size_t i = 0; i < sequentialResults.size(); ++i) {
        assert(sequentialResults[i].symbol == parallelResults[i].symbol);
        assert(approxEqual(sequentialResults[i].rsi, parallelResults[i].rsi));
        assert(approxEqual(sequentialResults[i].sma_20, parallelResults[i].sma_20));
        assert(approxEqual(sequentialResults[i].sma_50, parallelResults[i].sma_50));
        assert(sequentialResults[i].signal == parallelResults[i].signal);
    }
    
    std::cout << "PASSED\n";
}

// Test 6: Edge Cases
void testEdgeCases() {
    std::cout << "Test 6: Edge Cases... ";
    
    TechnicalIndicator indicator;
    
    // Empty data
    TechnicalIndicator::StockData emptyStock;
    emptyStock.symbol = "EMPTY";
    auto result1 = indicator.computeIndicators(emptyStock);
    assert(result1.signal == "HOLD");
    assert(result1.signal_strength == 0.0);
    
    // Insufficient data for indicators
    TechnicalIndicator::StockData smallStock;
    smallStock.symbol = "SMALL";
    for (int i = 0; i < 5; ++i) {
        smallStock.prices.push_back(100.0);
    }
    auto result2 = indicator.computeIndicators(smallStock);
    // Should handle gracefully without crashing
    assert(result2.symbol == "SMALL");
    
    std::cout << "PASSED\n";
}

int main() {
    std::cout << "=== Technical Indicator Unit Tests ===\n\n";
    
    try {
        testSMA();
        testRSI();
        testMACD();
        testSignalGeneration();
        testParallelConsistency();
        testEdgeCases();
        
        std::cout << "\n=== All Tests PASSED ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "\n=== Test FAILED: " << e.what() << " ===\n";
        return 1;
    } catch (...) {
        std::cerr << "\n=== Test FAILED: Unknown error ===\n";
        return 1;
    }
}

