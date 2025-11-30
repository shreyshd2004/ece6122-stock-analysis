#include "../include/Scheduler.h"
#include "../include/StockDataFetcher.h"
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
    
    schedulerThread_ = std::thread(&Scheduler::schedulerThread, this);
    dataFetcherThread_ = std::thread(&Scheduler::dataFetcherThread, this);
    notificationDispatcherThread_ = std::thread(&Scheduler::notificationDispatcherThread, this);
    
    std::cout << "[Scheduler] Started with interval: " << intervalSeconds_ << " seconds\n";
}

void Scheduler::stop() {
    if (!running_) {
        return;
    }

    shouldStop_ = true;
    running_ = false;
    
    dataQueue_.stop();
    notificationQueue_.stop();
    
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

void Scheduler::schedulerThread() {
    std::cout << "[Scheduler] Thread started\n";
    
    while (!shouldStop_) {
        auto startTime = std::chrono::steady_clock::now();
        
        {
            std::lock_guard<std::mutex> lock(cacheMutex_);
            if (!stockDataCache_.empty() && analysisCallback_) {
                std::cout << "[Scheduler] Triggering analysis cycle for " 
                          << stockDataCache_.size() << " stocks\n";
                analysisCallback_(stockDataCache_);
            }
        }
        
        auto targetTime = startTime + std::chrono::seconds(intervalSeconds_);
        while (std::chrono::steady_clock::now() < targetTime && !shouldStop_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    std::cout << "[Scheduler] Thread stopped\n";
}

void Scheduler::dataFetcherThread() {
    std::cout << "[DataFetcher] Thread started\n";
    std::cout << "[DataFetcher] Using socket connections for real-time data fetching\n";
    
    StockDataFetcher fetcher;
    fetcher.setTimeout(5);
    
    std::vector<std::string> symbols = {"IBM", "AAPL", "GOOGL", "MSFT", "AMZN", 
                                        "TSLA", "META", "NVDA", "JPM", "V"};
    
    while (!shouldStop_) {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        if (shouldStop_) break;
        
        std::cout << "[DataFetcher] Fetching data via socket connections...\n";
        
        for (const auto& symbol : symbols) {
            TechnicalIndicator::StockData stockData = fetcher.fetchStockData(symbol);
            dataQueue_.push(stockData);
        }
        
        std::cout << "[DataFetcher] Fetched data for " << symbols.size() 
                  << " stocks using socket connections\n";
    }
    
    std::cout << "[DataFetcher] Thread stopped\n";
}

void Scheduler::notificationDispatcherThread() {
    std::cout << "[NotificationDispatcher] Thread started\n";
    
    while (!shouldStop_) {
        auto result = notificationQueue_.tryPop();
        
        if (result.has_value()) {
            TechnicalIndicator::IndicatorResult notification = result.value();
            
            if (notification.signal == "BUY" || notification.signal == "SELL") {
                std::cout << "[NotificationDispatcher] Signal: " << notification.signal 
                          << " for " << notification.symbol 
                          << " (Strength: " << notification.signal_strength << ")\n";
                
                if (notificationCallback_) {
                    notificationCallback_(notification);
                }
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    std::cout << "[NotificationDispatcher] Thread stopped\n";
}

