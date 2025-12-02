#ifndef STOCK_DATA_FETCHER_H
#define STOCK_DATA_FETCHER_H

#include "TechnicalIndicator.h"
#include <string>
#include <vector>

class StockDataFetcher {
public:
    StockDataFetcher();
    ~StockDataFetcher();

    TechnicalIndicator::StockData fetchStockData(const std::string& symbol);
    std::vector<TechnicalIndicator::StockData> fetchMultipleStocks(
        const std::vector<std::string>& symbols);
    void setTimeout(int seconds) { timeoutSeconds_ = seconds; }

private:
    TechnicalIndicator::StockData generateSampleData(const std::string& symbol);

    int timeoutSeconds_;
};

#endif
