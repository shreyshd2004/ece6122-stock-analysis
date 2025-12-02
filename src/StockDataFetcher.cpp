#include "../include/StockDataFetcher.h"
#include <random>
#include <iostream>

StockDataFetcher::StockDataFetcher()
    : timeoutSeconds_(5) {
}

StockDataFetcher::~StockDataFetcher() {
}

TechnicalIndicator::StockData StockDataFetcher::generateSampleData(const std::string& symbol) {
    TechnicalIndicator::StockData stockData;
    stockData.symbol = symbol;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> priceDist(100.0, 200.0);
    std::uniform_real_distribution<double> volumeDist(1000000.0, 10000000.0);
    
    double basePrice = priceDist(gen);
    for (int day = 0; day < 100; ++day) {
        basePrice += (priceDist(gen) - basePrice) * 0.1;
        stockData.prices.push_back(basePrice);
        stockData.volumes.push_back(volumeDist(gen));
        stockData.timestamps.push_back(day);
    }
    
    return stockData;
}

TechnicalIndicator::StockData StockDataFetcher::fetchStockData(const std::string& symbol) {
    return generateSampleData(symbol);
}

std::vector<TechnicalIndicator::StockData> StockDataFetcher::fetchMultipleStocks(
    const std::vector<std::string>& symbols) {
    
    std::vector<TechnicalIndicator::StockData> results;
    results.reserve(symbols.size());
    
    for (const auto& symbol : symbols) {
        results.push_back(fetchStockData(symbol));
    }
    
    return results;
}
