#ifndef TECHNICAL_INDICATOR_H
#define TECHNICAL_INDICATOR_H

#include <vector>
#include <string>
#include <memory>

/**
 * TechnicalIndicator Class
 * 
 * Encapsulates technical indicator computation logic for stock analysis.
 * Designed to be parallelized using OpenMP for data parallelism.
 * 
 * Author: Hersh Gupta
 */
class TechnicalIndicator {
public:
    struct StockData {
        std::string symbol;
        std::vector<double> prices;
        std::vector<double> volumes;
        std::vector<double> timestamps;
    };

    struct IndicatorResult {
        std::string symbol;
        double sma_20;
        double sma_50;
        double rsi;
        double macd;
        double macd_signal;
        std::string signal; // "BUY", "SELL", or "HOLD"
        double signal_strength;
    };

    TechnicalIndicator();
    ~TechnicalIndicator();

    // Compute indicators for a single stock
    IndicatorResult computeIndicators(const StockData& stockData);

    // Parallel computation for multiple stocks (OpenMP)
    std::vector<IndicatorResult> computeIndicatorsParallel(
        const std::vector<StockData>& stocks);

private:
    // Technical indicator calculations
    double calculateSMA(const std::vector<double>& prices, int period);
    double calculateRSI(const std::vector<double>& prices, int period = 14);
    std::pair<double, double> calculateMACD(const std::vector<double>& prices);
    
    // Signal generation logic
    std::string generateSignal(const IndicatorResult& result);
    double calculateSignalStrength(const IndicatorResult& result);
};

#endif // TECHNICAL_INDICATOR_H

