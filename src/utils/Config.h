#pragma once

#include "../../include/common.h"

class Config {
private:
    static std::unique_ptr<Config> instance_;
    static std::mutex instance_mutex_;
    
    std::unordered_map<std::string, std::string> config_map_;
    mutable std::mutex config_mutex_;
    
    Config() = default;
    
public:
    static Config& getInstance();
    
    bool loadFromFile(const std::string& filename);
    void loadFromCommandLine(int argc, char* argv[]);
    
    template<typename T>
    T get(const std::string& key, const T& default_value = T{}) const {
        std::lock_guard<std::mutex> lock(config_mutex_);
        auto it = config_map_.find(key);
        if (it == config_map_.end()) {
            return default_value;
        }
        return convertValue<T>(it->second);
    }
    
    void set(const std::string& key, const std::string& value);
    bool has(const std::string& key) const;
    void printAll() const;
    
private:
    template<typename T>
    T convertValue(const std::string& value) const {
        if constexpr (std::is_same_v<T, std::string>) {
            return value;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::stoi(value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return value == "true" || value == "1" || value == "yes";
        } else {
            static_assert(sizeof(T) == 0, "Unsupported type for Config::get");
        }
    }
    
    void parseJsonValue(const std::string& prefix, const std::string& json);
    std::string trim(const std::string& str);
};