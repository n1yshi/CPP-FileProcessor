#pragma once

#include "../../include/common.h"

class Timer {
private:
    std::chrono::steady_clock::time_point start_time_;
    std::chrono::steady_clock::time_point end_time_;
    bool running_;
    
public:
    Timer() : running_(false) {}
    
    void start() {
        start_time_ = std::chrono::steady_clock::now();
        running_ = true;
    }
    
    void stop() {
        end_time_ = std::chrono::steady_clock::now();
        running_ = false;
    }
    
    double elapsed_seconds() const {
        auto end = running_ ? std::chrono::steady_clock::now() : end_time_;
        return std::chrono::duration<double>(end - start_time_).count();
    }
    
    std::chrono::milliseconds elapsed_milliseconds() const {
        auto end = running_ ? std::chrono::steady_clock::now() : end_time_;
        return std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time_);
    }
    
    std::chrono::microseconds elapsed_microseconds() const {
        auto end = running_ ? std::chrono::steady_clock::now() : end_time_;
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start_time_);
    }
    
    bool is_running() const {
        return running_;
    }
    
    void reset() {
        running_ = false;
        start_time_ = std::chrono::steady_clock::time_point{};
        end_time_ = std::chrono::steady_clock::time_point{};
    }
};

class ScopedTimer {
private:
    Timer& timer_;
    
public:
    explicit ScopedTimer(Timer& timer) : timer_(timer) {
        timer_.start();
    }
    
    ~ScopedTimer() {
        timer_.stop();
    }
};