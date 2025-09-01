#pragma once

#include "../../include/common.h"

class ThreadPool {
private:
    std::vector<std::thread> workers_;
    ThreadSafeQueue<std::function<void()>> tasks_;
    std::atomic<bool> stop_;
    std::atomic<size_t> active_tasks_;
    std::condition_variable finished_;
    std::mutex finished_mutex_;
    
public:
    explicit ThreadPool(size_t num_threads);
    ~ThreadPool();
    
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args) 
        -> std::future<typename std::result_of<F(Args...)>::type> {
        
        using return_type = typename std::result_of<F(Args...)>::type;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> result = task->get_future();
        
        if (stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }
        
        tasks_.push([this, task]() {
            ++active_tasks_;
            (*task)();
            --active_tasks_;
            finished_.notify_all();
        });
        
        return result;
    }
    
    void wait_for_all();
    void shutdown();
    size_t size() const;
    size_t active_count() const;
    size_t pending_count() const;
    
private:
    void worker_thread();
};