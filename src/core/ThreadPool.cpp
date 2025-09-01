#include "ThreadPool.h"
#include "../utils/Logger.h"

ThreadPool::ThreadPool(size_t num_threads) : stop_(false), active_tasks_(0) {
    Logger::getInstance().info("Creating ThreadPool with " + std::to_string(num_threads) + " threads");
    
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back(&ThreadPool::worker_thread, this);
    }
}

ThreadPool::~ThreadPool() {
    shutdown();
}

void ThreadPool::worker_thread() {
    while (!stop_) {
        std::function<void()> task;
        
        if (tasks_.try_pop(task)) {
            try {
                task();
            } catch (const std::exception& e) {
                Logger::getInstance().error("Task execution failed: " + std::string(e.what()));
            } catch (...) {
                Logger::getInstance().error("Task execution failed with unknown exception");
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

void ThreadPool::wait_for_all() {
    std::unique_lock<std::mutex> lock(finished_mutex_);
    finished_.wait(lock, [this] {
        return tasks_.empty() && active_tasks_.load() == 0;
    });
}

void ThreadPool::shutdown() {
    if (stop_) {
        return;
    }
    
    Logger::getInstance().info("Shutting down ThreadPool");
    stop_ = true;
    
    for (auto& worker : workers_) {
        if (worker.joinable()) {
            worker.join();
        }
    }
    
    workers_.clear();
}

size_t ThreadPool::size() const {
    return workers_.size();
}

size_t ThreadPool::active_count() const {
    return active_tasks_.load();
}

size_t ThreadPool::pending_count() const {
    return tasks_.size();
}