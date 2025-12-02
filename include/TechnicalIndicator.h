#ifndef TECHNICAL_INDICATOR_H
#define TECHNICAL_INDICATOR_H

#include <vector>
#include <string>
#include <memory>

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
        std::string signal;
        double signal_strength;
    };

    TechnicalIndicator();
    ~TechnicalIndicator();

    IndicatorResult computeIndicators(const StockData& stockData);
    std::vector<IndicatorResult> computeIndicatorsParallel(
        const std::vector<StockData>& stocks);

private:
    double calculateSMA(const std::vector<double>& prices, int period);
    double calculateRSI(const std::vector<double>& prices, int period = 14);
    std::pair<double, double> calculateMACD(const std::vector<double>& prices);
    std::string generateSignal(const IndicatorResult& result);
    double calculateSignalStrength(const IndicatorResult& result);
};

#endif
