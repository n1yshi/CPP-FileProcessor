#include "ProgressMonitor.h"
#include "../utils/Logger.h"

ProgressMonitor::ProgressMonitor(bool verbose) 
    : total_files_(0), completed_files_(0), total_bytes_(0), processed_bytes_(0),
      start_time_(std::chrono::steady_clock::now()), verbose_(verbose) {}

void ProgressMonitor::notify(const ProgressEvent& event) {
    processed_bytes_ += event.bytes_processed;
    
    if (event.status == "completed") {
        ++completed_files_;
    }
    
    if (verbose_) {
        Logger::getInstance().info("Processing: " + event.filename + " (" + 
                                 std::to_string(event.percentage) + "%)");
    }
    
    if (!verbose_) {
        print_progress_bar();
    }
}

void ProgressMonitor::set_totals(size_t files, size_t bytes) {
    total_files_ = files;
    total_bytes_ = bytes;
}

void ProgressMonitor::print_summary() const {
    auto now = std::chrono::steady_clock::now();
    double duration = std::chrono::duration<double>(now - start_time_).count();
    
    std::lock_guard<std::mutex> lock(display_mutex_);
    
    std::cout << "\n=== Processing Summary ===\n";
    std::cout << "Files processed: " << completed_files_.load() << "/" << total_files_.load() << "\n";
    std::cout << "Bytes processed: " << format_bytes(processed_bytes_.load()) << "/" 
              << format_bytes(total_bytes_.load()) << "\n";
    std::cout << "Duration: " << format_duration(duration) << "\n";
    
    if (duration > 0) {
        double throughput = processed_bytes_.load() / (1024.0 * 1024.0) / duration;
        std::cout << "Throughput: " << std::fixed << std::setprecision(2) 
                  << throughput << " MB/s\n";
    }
    
    std::cout << "==========================\n";
}

void ProgressMonitor::print_progress_bar() const {
    std::lock_guard<std::mutex> lock(display_mutex_);
    
    double file_progress = total_files_.load() > 0 ? 
        static_cast<double>(completed_files_.load()) / total_files_.load() * 100.0 : 0.0;
    
    double byte_progress = total_bytes_.load() > 0 ? 
        static_cast<double>(processed_bytes_.load()) / total_bytes_.load() * 100.0 : 0.0;
    
    const int bar_width = 50;
    int pos = static_cast<int>(file_progress / 100.0 * bar_width);
    
    clear_line();
    std::cout << "\r[";
    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << file_progress << "% "
              << "(" << completed_files_.load() << "/" << total_files_.load() << " files, "
              << format_bytes(processed_bytes_.load()) << "/" << format_bytes(total_bytes_.load()) << ")";
    std::cout.flush();
}

std::string ProgressMonitor::format_bytes(size_t bytes) const {
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && unit < 4) {
        size /= 1024.0;
        ++unit;
    }
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << size << " " << units[unit];
    return oss.str();
}

std::string ProgressMonitor::format_duration(double seconds) const {
    int hours = static_cast<int>(seconds) / 3600;
    int minutes = (static_cast<int>(seconds) % 3600) / 60;
    int secs = static_cast<int>(seconds) % 60;
    
    std::ostringstream oss;
    if (hours > 0) {
        oss << hours << "h ";
    }
    if (minutes > 0 || hours > 0) {
        oss << minutes << "m ";
    }
    oss << secs << "s";
    return oss.str();
}

void ProgressMonitor::clear_line() const {
    std::cout << "\r" << std::string(80, ' ') << "\r";
}