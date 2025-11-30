#include "../include/StockDataFetcher.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <random>
#include <iostream>
#include <algorithm>
#include <regex>

StockDataFetcher::StockDataFetcher()
    : socketAvailable_(false), timeoutSeconds_(5), socketFd_(-1) {
}

StockDataFetcher::~StockDataFetcher() {
    disconnect();
}

bool StockDataFetcher::connectToServer(const std::string& host, int port) {
    socketFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd_ < 0) {
        std::cerr << "[StockDataFetcher] Error creating socket\n";
        return false;
    }

    struct hostent* server = gethostbyname(host.c_str());
    if (server == nullptr) {
        std::cerr << "[StockDataFetcher] Error: No such host\n";
        close(socketFd_);
        socketFd_ = -1;
        return false;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    struct timeval timeout;
    timeout.tv_sec = timeoutSeconds_;
    timeout.tv_usec = 0;
    setsockopt(socketFd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(socketFd_, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    if (connect(socketFd_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "[StockDataFetcher] Error connecting to " << host << ":" << port << "\n";
        close(socketFd_);
        socketFd_ = -1;
        return false;
    }

    socketAvailable_ = true;
    return true;
}

void StockDataFetcher::disconnect() {
    if (socketFd_ >= 0) {
        close(socketFd_);
        socketFd_ = -1;
    }
    socketAvailable_ = false;
}

std::string StockDataFetcher::sendHTTPRequest(const std::string& host, const std::string& path) {
    if (socketFd_ < 0) {
        if (!connectToServer(host, API_PORT)) {
            return "";
        }
    }

    std::ostringstream request;
    request << "GET " << path << " HTTP/1.1\r\n";
    request << "Host: " << host << "\r\n";
    request << "User-Agent: Mozilla/5.0 (compatible; StockAnalyzer/1.0)\r\n";
    request << "Accept: application/json\r\n";
    request << "Connection: close\r\n";
    request << "\r\n";
    
    // Note: For HTTPS (port 443), TLS/SSL handshake is required before sending HTTP request
    // This requires OpenSSL or similar library for full implementation

    std::string requestStr = request.str();

    if (send(socketFd_, requestStr.c_str(), requestStr.length(), 0) < 0) {
        std::cerr << "[StockDataFetcher] Error sending request\n";
        disconnect();
        return "";
    }

    std::string response;
    char buffer[4096];
    int bytesReceived;

    while ((bytesReceived = recv(socketFd_, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesReceived] = '\0';
        response += buffer;
    }

    disconnect();
    return response;
}

std::string StockDataFetcher::parseJSONResponse(const std::string& response, const std::string& /*symbol*/) {
    size_t jsonStart = response.find("\r\n\r\n");
    if (jsonStart == std::string::npos) {
        jsonStart = response.find("\n\n");
    }
    if (jsonStart != std::string::npos) {
        std::string jsonBody = response.substr(jsonStart);
        size_t firstBrace = jsonBody.find('{');
        if (firstBrace != std::string::npos) {
            return jsonBody.substr(firstBrace);
        }
    }
    return response;
}

std::vector<double> extractJSONArray(const std::string& json, const std::string& key) {
    std::vector<double> values;
    
    std::string searchKey = "\"" + key + "\":";
    size_t keyPos = json.find(searchKey);
    if (keyPos == std::string::npos) {
        return values;
    }
    
    size_t arrayStart = json.find('[', keyPos);
    if (arrayStart == std::string::npos) {
        return values;
    }
    
    int bracketCount = 0;
    size_t arrayEnd = arrayStart;
    for (size_t i = arrayStart; i < json.length(); ++i) {
        if (json[i] == '[') bracketCount++;
        if (json[i] == ']') bracketCount--;
        if (bracketCount == 0) {
            arrayEnd = i;
            break;
        }
    }
    
    if (arrayEnd == arrayStart) {
        return values;
    }
    
    std::string arrayContent = json.substr(arrayStart + 1, arrayEnd - arrayStart - 1);
    
    std::regex numberRegex(R"(-?\d+\.?\d*)");
    std::sregex_iterator iter(arrayContent.begin(), arrayContent.end(), numberRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        try {
            double value = std::stod(iter->str());
            if (value > 0 || key == "timestamp") {
                values.push_back(value);
            }
        } catch (...) {
            continue;
        }
    }
    
    return values;
}

TechnicalIndicator::StockData StockDataFetcher::parseStockData(
    const std::string& symbol, const std::string& jsonData) {
    
    TechnicalIndicator::StockData stockData;
    stockData.symbol = symbol;
    
    if (jsonData.empty()) {
        return generateSampleData(symbol);
    }
    
    // Check for Alpha Vantage error messages
    if (jsonData.find("\"Error Message\"") != std::string::npos || 
        jsonData.find("\"Note\"") != std::string::npos) {
        std::cout << "[StockDataFetcher] API error or rate limit for " << symbol << "\n";
        return generateSampleData(symbol);
    }
    
    // Alpha Vantage format: "Time Series (5min)": { "timestamp": { "4. close": "price", "5. volume": "vol" } }
    std::string timeSeriesKey = "\"Time Series (5min)\"";
    size_t timeSeriesPos = jsonData.find(timeSeriesKey);
    
    if (timeSeriesPos == std::string::npos) {
        timeSeriesKey = "\"Time Series (1min)\"";
        timeSeriesPos = jsonData.find(timeSeriesKey);
    }
    if (timeSeriesPos == std::string::npos) {
        timeSeriesKey = "\"Time Series (15min)\"";
        timeSeriesPos = jsonData.find(timeSeriesKey);
    }
    if (timeSeriesPos == std::string::npos) {
        timeSeriesKey = "\"Time Series (60min)\"";
        timeSeriesPos = jsonData.find(timeSeriesKey);
    }
    
    if (timeSeriesPos == std::string::npos) {
        std::cout << "[StockDataFetcher] Time Series not found in JSON for " << symbol << "\n";
        return generateSampleData(symbol);
    }
    
    // Extract all close prices and volumes from the time series
    std::vector<double> prices;
    std::vector<double> volumes;
    std::vector<double> timestamps;
    
    size_t seriesStart = jsonData.find('{', timeSeriesPos);
    if (seriesStart == std::string::npos) {
        return generateSampleData(symbol);
    }
    
    // Parse each timestamp entry: "2024-01-01 09:30:00": { "4. close": "150.25", "5. volume": "1000000" }
    std::regex timestampRegex(R"(\"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\":\s*\{)");
    std::regex closeRegex(R"(\"4\. close\":\s*\"([0-9.]+)\")");
    std::regex volumeRegex(R"(\"5\. volume\":\s*\"([0-9.]+)\")");
    
    std::sregex_iterator timestampIter(jsonData.begin() + seriesStart, jsonData.end(), timestampRegex);
    std::sregex_iterator end;
    
    int entryCount = 0;
    for (; timestampIter != end && entryCount < 100; ++timestampIter, ++entryCount) {
        size_t entryStart = timestampIter->position() + timestampIter->length();
        size_t entryEnd = jsonData.find('}', entryStart);
        if (entryEnd == std::string::npos) break;
        
        std::string entry = jsonData.substr(entryStart, entryEnd - entryStart);
        
        std::sregex_iterator closeIter(entry.begin(), entry.end(), closeRegex);
        std::sregex_iterator volumeIter(entry.begin(), entry.end(), volumeRegex);
        
        if (closeIter != std::sregex_iterator()) {
            try {
                double closePrice = std::stod(closeIter->str(1));
                prices.push_back(closePrice);
                
                if (volumeIter != std::sregex_iterator()) {
                    double volume = std::stod(volumeIter->str(1));
                    volumes.push_back(volume);
                } else {
                    volumes.push_back(0.0);
                }
                
                timestamps.push_back(entryCount);
            } catch (...) {
                continue;
            }
        }
    }
    
    // Reverse to get chronological order (oldest first)
    std::reverse(prices.begin(), prices.end());
    std::reverse(volumes.begin(), volumes.end());
    
    if (prices.size() >= 20) {
        stockData.prices = prices;
        stockData.volumes = volumes;
        for (size_t i = 0; i < prices.size(); ++i) {
            stockData.timestamps.push_back(i);
        }
        
        std::cout << "[StockDataFetcher] Successfully parsed " << stockData.prices.size() 
                  << " data points from Alpha Vantage API for " << symbol << "\n";
        return stockData;
    }
    
    std::cout << "[StockDataFetcher] Insufficient data points (" << prices.size() 
              << ") from Alpha Vantage for " << symbol << "\n";
    return generateSampleData(symbol);
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
    std::ostringstream pathStream;
    pathStream << "/query?function=TIME_SERIES_INTRADAY&symbol=" << symbol 
               << "&interval=5min&apikey=" << API_KEY;
    std::string path = pathStream.str();
    
    std::string response = sendHTTPRequest(API_HOST, path);
    
    if (response.empty()) {
        std::cout << "[StockDataFetcher] Empty response for " << symbol 
                  << ", using sample data\n";
        return generateSampleData(symbol);
    }
    
    if (response.find("200 OK") != std::string::npos || response.find('{') != std::string::npos) {
        std::string jsonData = parseJSONResponse(response, symbol);
        
        if (!jsonData.empty() && (jsonData.find("Time Series") != std::string::npos || 
                                  jsonData.find("\"Meta Data\"") != std::string::npos)) {
            TechnicalIndicator::StockData data = parseStockData(symbol, jsonData);
            
            if (!data.prices.empty() && data.prices.size() >= 20) {
                std::cout << "[StockDataFetcher] Successfully fetched and parsed real-time data from Alpha Vantage for " 
                          << symbol << " (" << data.prices.size() << " data points)\n";
                return data;
            }
        }
    }
    
    std::cout << "[StockDataFetcher] Could not parse real-time data for " << symbol 
              << ", using sample data\n";
    return generateSampleData(symbol);
}

std::vector<TechnicalIndicator::StockData> StockDataFetcher::fetchMultipleStocks(
    const std::vector<std::string>& symbols) {
    
    std::vector<TechnicalIndicator::StockData> results;
    results.reserve(symbols.size());
    
    for (const auto& symbol : symbols) {
        results.push_back(fetchStockData(symbol));
        usleep(100000);
    }
    
    return results;
}

