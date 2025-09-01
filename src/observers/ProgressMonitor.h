#pragma once

#include "Observer.h"

class ProgressMonitor : public Observer<ProgressEvent> {
private:
    std::atomic<size_t> total_files_;
    std::atomic<size_t> completed_files_;
    std::atomic<size_t> total_bytes_;
    std::atomic<size_t> processed_bytes_;
    std::chrono::steady_clock::time_point start_time_;
    mutable std::mutex display_mutex_;
    bool verbose_;
    
public:
    explicit ProgressMonitor(bool verbose = false);
    
    void notify(const ProgressEvent& event) override;
    void set_totals(size_t files, size_t bytes);
    void print_summary() const;
    void print_progress_bar() const;
    
private:
    std::string format_bytes(size_t bytes) const;
    std::string format_duration(double seconds) const;
    void clear_line() const;
};