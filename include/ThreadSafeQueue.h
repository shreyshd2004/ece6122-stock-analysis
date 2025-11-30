#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <optional>

/**
 * ThreadSafeQueue Class
 * 
 * A thread-safe queue implementation using mutexes and condition variables
 * for safe inter-thread communication.
 * 
 * Author: Shreyashi Dutta
 */
template<typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;

    // Non-copyable
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    // Push an item to the queue
    void push(const T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(item);
        condition_.notify_one();
    }

    void push(T&& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(std::move(item));
        condition_.notify_one();
    }

    // Pop an item from the queue (blocks until item is available)
    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return !queue_.empty() || stop_; });
        
        if (stop_ && queue_.empty()) {
            return T{}; // Return default value if stopped
        }
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    // Try to pop an item (non-blocking)
    std::optional<T> tryPop() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return std::nullopt;
        }
        
        T item = std::move(queue_.front());
        queue_.pop();
        return item;
    }

    // Check if queue is empty
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    // Get queue size
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    // Signal all waiting threads to stop
    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = true;
        condition_.notify_all();
    }

    // Reset stop flag
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        stop_ = false;
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable condition_;
    std::queue<T> queue_;
    bool stop_ = false;
};

#endif // THREAD_SAFE_QUEUE_H

