#ifndef STOCK_DATA_FETCHER_H
#define STOCK_DATA_FETCHER_H

#include "TechnicalIndicator.h"
#include <string>
#include <vector>

/**
 * StockDataFetcher Class
 * 
 * Handles fetching stock market data using socket connections.
 * Demonstrates network programming with sockets for real-time data retrieval.
 * 
 * Author: Shreyashi Dutta (Extended with Sockets)
 */
class StockDataFetcher {
public:
    StockDataFetcher();
    ~StockDataFetcher();

    // Fetch stock data using socket connection
    TechnicalIndicator::StockData fetchStockData(const std::string& symbol);
    
    // Fetch multiple stocks in parallel
    std::vector<TechnicalIndicator::StockData> fetchMultipleStocks(
        const std::vector<std::string>& symbols);

    // Check if socket connection is available
    bool isSocketAvailable() const { return socketAvailable_; }

    // Set timeout for socket operations (in seconds)
    void setTimeout(int seconds) { timeoutSeconds_ = seconds; }

private:
    // Socket connection methods
    bool connectToServer(const std::string& host, int port);
    void disconnect();
    std::string sendHTTPRequest(const std::string& host, const std::string& path);
    std::string parseJSONResponse(const std::string& response, const std::string& symbol);
    
    // Parse stock data from API response
    TechnicalIndicator::StockData parseStockData(
        const std::string& symbol, const std::string& jsonData);
    
    // Fallback: Generate sample data if socket fails
    TechnicalIndicator::StockData generateSampleData(const std::string& symbol);

    bool socketAvailable_;
    int timeoutSeconds_;
    int socketFd_;
    
    // API configuration - Alpha Vantage API
    static constexpr const char* API_HOST = "www.alphavantage.co";
    static constexpr int API_PORT = 443;  // HTTPS port
    static constexpr const char* API_KEY = "P0HB29ZLXIPNGG3U";
    
    // Note: Alpha Vantage uses HTTPS (port 443) which requires TLS/SSL
    // For basic socket demonstration, this shows the HTTP request structure
    // Full HTTPS requires TLS implementation (OpenSSL or similar library)
};

#endif // STOCK_DATA_FETCHER_H

