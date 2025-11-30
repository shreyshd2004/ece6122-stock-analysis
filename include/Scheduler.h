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

/**
 * Scheduler Class
 * 
 * Manages recurring analysis intervals and triggers the OpenMP computation engine.
 * Coordinates background operations independently from the compute layer.
 * 
 * Author: Shreyashi Dutta
 */
class Scheduler {
public:
    using AnalysisCallback = std::function<void(const std::vector<TechnicalIndicator::StockData>&)>;
    using NotificationCallback = std::function<void(const TechnicalIndicator::IndicatorResult&)>;

    Scheduler(int intervalSeconds = 3600); // Default: 1 hour
    ~Scheduler();

    // Start the scheduler
    void start();

    // Stop the scheduler gracefully
    void stop();

    // Register callback for analysis cycles
    void setAnalysisCallback(AnalysisCallback callback);

    // Register callback for notifications
    void setNotificationCallback(NotificationCallback callback);

    // Add stock data to the analysis queue
    void addStockData(const TechnicalIndicator::StockData& stockData);

    // Get the notification queue (for dispatcher thread)
    ThreadSafeQueue<TechnicalIndicator::IndicatorResult>& getNotificationQueue();

    // Check if scheduler is running
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

#endif // SCHEDULER_H

