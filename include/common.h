#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include <functional>
#include <atomic>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <filesystem>
#include <exception>
#include <typeinfo>

namespace fs = std::filesystem;

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3
};

enum class ProcessorType {
    TEXT,
    IMAGE,
    AUTO
};

struct ProcessResult {
    bool success;
    std::string message;
    size_t bytes_processed;
    std::chrono::milliseconds processing_time;
    std::unordered_map<std::string, std::string> metadata;
    
    ProcessResult() : success(false), bytes_processed(0), processing_time(0) {}
};

struct ProcessingStats {
    std::atomic<size_t> files_processed{0};
    std::atomic<size_t> bytes_processed{0};
    std::atomic<size_t> errors{0};
    std::chrono::steady_clock::time_point start_time;
    std::chrono::steady_clock::time_point end_time;
    
    ProcessingStats() : start_time(std::chrono::steady_clock::now()) {}
    
    double get_duration_seconds() const {
        auto end = (end_time == std::chrono::steady_clock::time_point{}) ? 
                   std::chrono::steady_clock::now() : end_time;
        return std::chrono::duration<double>(end - start_time).count();
    }
    
    double get_throughput_mbps() const {
        double duration = get_duration_seconds();
        return duration > 0 ? (bytes_processed.load() / (1024.0 * 1024.0)) / duration : 0.0;
    }
};

template<typename T>
class ThreadSafeQueue {
private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
    std::condition_variable condition_;

public:
    void push(T item) {
        std::lock_guard<std::mutex> lock(mutex_);
        queue_.push(item);
        condition_.notify_one();
    }
    
    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lock(mutex_);
        if(queue_.empty()) {
            return false;
        }
        item = queue_.front();
        queue_.pop();
        return true;
    }
    
    void wait_and_pop(T& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        while(queue_.empty()) {
            condition_.wait(lock);
        }
        item = queue_.front();
        queue_.pop();
    }
    
    bool empty() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }
    
    size_t size() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }
};