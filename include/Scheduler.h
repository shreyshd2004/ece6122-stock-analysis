#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "ThreadSafeQueue.h"
#include "TechnicalIndicator.h"
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <functional>
#include <memory>

class Scheduler {
public:
    using AnalysisCallback = std::function<void(const std::vector<TechnicalIndicator::StockData>&)>;
    using NotificationCallback = std::function<void(const TechnicalIndicator::IndicatorResult&)>;

    Scheduler(int intervalSeconds = 3600);
    ~Scheduler();

    void start();
    void stop();
    void setAnalysisCallback(AnalysisCallback callback);
    void setNotificationCallback(NotificationCallback callback);
    void addStockData(const TechnicalIndicator::StockData& stockData);
    ThreadSafeQueue<TechnicalIndicator::IndicatorResult>& getNotificationQueue();
    bool isRunning() const { return running_; }

private:
    void schedulerThread();
    void dataFetcherThread();
    void notificationDispatcherThread();

    int intervalSeconds_;
    std::atomic<bool> running_;
    std::atomic<bool> shouldStop_;

    std::thread schedulerThread_;
    std::thread dataFetcherThread_;
    std::thread notificationDispatcherThread_;

    AnalysisCallback analysisCallback_;
    NotificationCallback notificationCallback_;

    ThreadSafeQueue<TechnicalIndicator::StockData> dataQueue_;
    ThreadSafeQueue<TechnicalIndicator::IndicatorResult> notificationQueue_;
    
    std::vector<TechnicalIndicator::StockData> stockDataCache_;
    std::mutex cacheMutex_;
};

#endif
