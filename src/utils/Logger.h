#pragma once

#include "../../include/common.h"

class Logger {
private:
    static std::unique_ptr<Logger> instance_;
    static std::mutex instance_mutex_;
    
    std::mutex log_mutex_;
    std::ofstream log_file_;
    LogLevel current_level_;
    bool console_output_;
    
    Logger();
    
public:
    static Logger& getInstance();
    
    void setLevel(LogLevel level);
    void setConsoleOutput(bool enable);
    void setLogFile(const std::string& filename);
    
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);
    
    template<typename... Args>
    void logf(LogLevel level, const std::string& format, Args... args) {
        std::ostringstream oss;
        logf_impl(oss, format, args...);
        log(level, oss.str());
    }
    
private:
    std::string levelToString(LogLevel level);
    std::string getCurrentTimestamp();
    
    template<typename T>
    void logf_impl(std::ostringstream& oss, const std::string& format, T&& t) {
        size_t pos = format.find("{}");
        if (pos != std::string::npos) {
            oss << format.substr(0, pos) << std::forward<T>(t) << format.substr(pos + 2);
        } else {
            oss << format;
        }
    }
    
    template<typename T, typename... Args>
    void logf_impl(std::ostringstream& oss, const std::string& format, T&& t, Args&&... args) {
        size_t pos = format.find("{}");
        if (pos != std::string::npos) {
            std::string new_format = format.substr(0, pos) + std::to_string(t) + format.substr(pos + 2);
            logf_impl(oss, new_format, std::forward<Args>(args)...);
        } else {
            oss << format;
        }
    }
};