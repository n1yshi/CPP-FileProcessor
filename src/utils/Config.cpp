#include "Config.h"

std::unique_ptr<Config> Config::instance_ = nullptr;
std::mutex Config::instance_mutex_;

Config& Config::getInstance() {
    std::lock_guard<std::mutex> lock(instance_mutex_);
    if (!instance_) {
        instance_ = std::unique_ptr<Config>(new Config());
    }
    return *instance_;
}

bool Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.length() - 2);
            }
            
            set(key, value);
        }
    }
    
    return true;
}

void Config::loadFromCommandLine(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg.starts_with("--")) {
            std::string key = arg.substr(2);
            std::string value = "true";
            
            size_t pos = key.find('=');
            if (pos != std::string::npos) {
                value = key.substr(pos + 1);
                key = key.substr(0, pos);
            } else if (i + 1 < argc && !std::string(argv[i + 1]).starts_with("-")) {
                value = argv[++i];
            }
            
            set(key, value);
        } else if (arg.starts_with("-") && arg.length() == 2) {
            char short_opt = arg[1];
            std::string value = "true";
            
            if (i + 1 < argc && !std::string(argv[i + 1]).starts_with("-")) {
                value = argv[++i];
            }
            
            switch (short_opt) {
                case 'i': set("input", value); break;
                case 'o': set("output", value); break;
                case 't': set("threads", value); break;
                case 'c': set("config", value); break;
                case 'v': set("verbose", "true"); break;
                case 's': set("stats", "true"); break;
                case 'h': set("help", "true"); break;
            }
        }
    }
}

void Config::set(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    config_map_[key] = value;
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(config_mutex_);
    return config_map_.find(key) != config_map_.end();
}

void Config::printAll() const {
    std::lock_guard<std::mutex> lock(config_mutex_);
    std::cout << "Configuration:\n";
    for (const auto& [key, value] : config_map_) {
        std::cout << "  " << key << " = " << value << "\n";
    }
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}