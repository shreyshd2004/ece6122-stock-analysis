#include "../include/TechnicalIndicator.h"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iostream>
#ifdef _OPENMP
#include <omp.h>
#endif

TechnicalIndicator::TechnicalIndicator() {
}

TechnicalIndicator::~TechnicalIndicator() {
}

double TechnicalIndicator::calculateSMA(const std::vector<double>& prices, int period) {
    if (prices.size() < static_cast<size_t>(period)) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (int i = prices.size() - period; i < static_cast<int>(prices.size()); ++i) {
        sum += prices[i];
    }
    return sum / period;
}

double TechnicalIndicator::calculateRSI(const std::vector<double>& prices, int period) {
    if (prices.size() < static_cast<size_t>(period + 1)) {
        return 50.0;
    }
    
    std::vector<double> gains, losses;
    for (size_t i = prices.size() - period; i < prices.size() - 1; ++i) {
        double change = prices[i + 1] - prices[i];
        if (change > 0) {
            gains.push_back(change);
            losses.push_back(0.0);
        } else {
            gains.push_back(0.0);
            losses.push_back(-change);
        }
    }
    
    double avgGain = std::accumulate(gains.begin(), gains.end(), 0.0) / period;
    double avgLoss = std::accumulate(losses.begin(), losses.end(), 0.0) / period;
    
    if (avgLoss == 0.0) {
        return 100.0;
    }
    
    double rs = avgGain / avgLoss;
    return 100.0 - (100.0 / (1.0 + rs));
}

std::pair<double, double> TechnicalIndicator::calculateMACD(const std::vector<double>& prices) {
    if (prices.size() < 26) {
        return {0.0, 0.0};
    }
    
    double ema12 = prices[prices.size() - 12];
    double ema26 = prices[prices.size() - 26];
    
    double multiplier12 = 2.0 / (12 + 1);
    double multiplier26 = 2.0 / (26 + 1);
    
    for (size_t i = prices.size() - 12 + 1; i < prices.size(); ++i) {
        ema12 = (prices[i] - ema12) * multiplier12 + ema12;
    }
    
    for (size_t i = prices.size() - 26 + 1; i < prices.size(); ++i) {
        ema26 = (prices[i] - ema26) * multiplier26 + ema26;
    }
    
    double macd = ema12 - ema26;
    double signal = macd * 0.9;
    
    return {macd, signal};
}

std::string TechnicalIndicator::generateSignal(const IndicatorResult& result) {
    int buySignals = 0;
    int sellSignals = 0;
    
    if (result.rsi < 30) buySignals++;
    else if (result.rsi > 70) sellSignals++;
    
    if (result.macd > result.macd_signal) buySignals++;
    else if (result.macd < result.macd_signal) sellSignals++;
    
    if (result.sma_20 > result.sma_50) buySignals++;
    else if (result.sma_20 < result.sma_50) sellSignals++;
    
    if (buySignals > sellSignals) return "BUY";
    if (sellSignals > buySignals) return "SELL";
    return "HOLD";
}

double TechnicalIndicator::calculateSignalStrength(const IndicatorResult& result) {
    double strength = 0.0;
    
    if (result.rsi < 30) strength += (30 - result.rsi) / 30.0;
    else if (result.rsi > 70) strength += (result.rsi - 70) / 30.0;
    
    double macdDiff = std::abs(result.macd - result.macd_signal);
    strength += macdDiff * 10.0;
    
    double smaDiff = std::abs(result.sma_20 - result.sma_50) / result.sma_50;
    strength += smaDiff * 100.0;
    
    return std::min(strength, 100.0);
}

TechnicalIndicator::IndicatorResult TechnicalIndicator::computeIndicators(
    const StockData& stockData) {
    
    IndicatorResult result;
    result.symbol = stockData.symbol;
    
    if (stockData.prices.empty()) {
        result.signal = "HOLD";
        result.signal_strength = 0.0;
        return result;
    }
    
    result.sma_20 = calculateSMA(stockData.prices, 20);
    result.sma_50 = calculateSMA(stockData.prices, 50);
    result.rsi = calculateRSI(stockData.prices, 14);
    
    auto macdPair = calculateMACD(stockData.prices);
    result.macd = macdPair.first;
    result.macd_signal = macdPair.second;
    
    result.signal = generateSignal(result);
    result.signal_strength = calculateSignalStrength(result);
    
    return result;
}

std::vector<TechnicalIndicator::IndicatorResult> 
TechnicalIndicator::computeIndicatorsParallel(
    const std::vector<StockData>& stocks) {
    
    std::vector<IndicatorResult> results(stocks.size());
    
    #ifdef _OPENMP
    #pragma omp parallel for
    for (size_t i = 0; i < stocks.size(); ++i) {
        results[i] = computeIndicators(stocks[i]);
    }
    #else
    for (size_t i = 0; i < stocks.size(); ++i) {
        results[i] = computeIndicators(stocks[i]);
    }
    #endif
    
    return results;
}

