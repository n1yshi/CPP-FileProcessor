#pragma once

#include "../../include/common.h"

template<typename EventType>
class Observer {
public:
    virtual ~Observer() = default;
    virtual void notify(const EventType& event) = 0;
};

template<typename EventType>
class Subject {
private:
    std::vector<std::weak_ptr<Observer<EventType>>> observers_;
    mutable std::mutex observers_mutex_;
    
public:
    void attach(std::shared_ptr<Observer<EventType>> observer) {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_.push_back(observer);
    }
    
    void detach(std::shared_ptr<Observer<EventType>> observer) {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        observers_.erase(
            std::remove_if(observers_.begin(), observers_.end(),
                [&observer](const std::weak_ptr<Observer<EventType>>& weak_obs) {
                    return weak_obs.expired() || weak_obs.lock() == observer;
                }),
            observers_.end()
        );
    }
    
    void notify_all(const EventType& event) {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        
        auto it = observers_.begin();
        while (it != observers_.end()) {
            if (auto observer = it->lock()) {
                try {
                    observer->notify(event);
                    ++it;
                } catch (const std::exception& e) {
                    Logger::getInstance().error("Observer notification failed: " + std::string(e.what()));
                    ++it;
                }
            } else {
                it = observers_.erase(it);
            }
        }
    }
    
    size_t observer_count() const {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        return std::count_if(observers_.begin(), observers_.end(),
            [](const std::weak_ptr<Observer<EventType>>& weak_obs) {
                return !weak_obs.expired();
            });
    }
};

struct ProgressEvent {
    std::string filename;
    size_t bytes_processed;
    size_t total_bytes;
    double percentage;
    std::string status;
    std::chrono::steady_clock::time_point timestamp;
    
    ProgressEvent(const std::string& file, size_t processed, size_t total, const std::string& stat)
        : filename(file), bytes_processed(processed), total_bytes(total), 
          status(stat), timestamp(std::chrono::steady_clock::now()) {
        percentage = total > 0 ? (static_cast<double>(processed) / total) * 100.0 : 0.0;
    }
};