#include "../include/common.h"
#include "utils/Logger.h"
#include "utils/Config.h"
#include "utils/Timer.h"
#include "core/ThreadPool.h"
#include "core/FileProcessor.h"
#include "processors/TextProcessor.h"
#include "observers/ProgressMonitor.h"

void print_help() {
    std::cout << "Multi-threaded File Processing System\n\n";
    std::cout << "Usage: file_processor [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -i, --input PATH      Input file or directory (required)\n";
    std::cout << "  -o, --output PATH     Output directory (default: ./output)\n";
    std::cout << "  -t, --threads NUM     Number of worker threads (default: 4)\n";
    std::cout << "  --type TYPE           Processor type: text, image, auto (default: auto)\n";
    std::cout << "  -c, --config PATH     Configuration file path\n";
    std::cout << "  -v, --verbose         Enable verbose logging\n";
    std::cout << "  -s, --stats           Show performance statistics\n";
    std::cout << "  -h, --help            Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  file_processor -i data/sample.txt -t 4\n";
    std::cout << "  file_processor -i data/files/ -o results/ -v -s\n";
}

std::vector<std::string> collect_files(const std::string& input_path) {
    std::vector<std::string> files;
    
    if (fs::is_regular_file(input_path)) {
        files.push_back(input_path);
    } else if (fs::is_directory(input_path)) {
        for (const auto& entry : fs::recursive_directory_iterator(input_path)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    }
    
    return files;
}

size_t calculate_total_size(const std::vector<std::string>& files) {
    size_t total = 0;
    for (const auto& file : files) {
        try {
            total += fs::file_size(file);
        } catch (const std::exception& e) {
            Logger::getInstance().warning("Cannot get size of file: " + file);
        }
    }
    return total;
}

std::unique_ptr<IFileProcessor> create_processor(const std::string& type, const std::string& output_dir) {
    if (type == "text") {
        return std::make_unique<TextProcessor>(output_dir);
    }
    
    return std::make_unique<TextProcessor>(output_dir);
}

ProcessorType determine_processor_type(const std::string& filepath) {
    fs::path path(filepath);
    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    static const std::unordered_set<std::string> text_extensions = {
        ".txt", ".md", ".csv", ".log", ".json", ".xml", ".html", ".css", ".js"
    };
    
    if (text_extensions.find(extension) != text_extensions.end()) {
        return ProcessorType::TEXT;
    }
    
    return ProcessorType::TEXT;
}

int main(int argc, char* argv[]) {
    try {
        Config& config = Config::getInstance();
        config.loadFromCommandLine(argc, argv);
        
        if (config.has("help")) {
            print_help();
            return 0;
        }
        
        if (!config.has("input")) {
            std::cerr << "Error: Input file or directory is required\n";
            print_help();
            return 1;
        }
        
        if (config.has("config")) {
            config.loadFromFile(config.get<std::string>("config"));
        }
        
        Logger& logger = Logger::getInstance();
        if (config.get<bool>("verbose")) {
            logger.setLevel(LogLevel::DEBUG);
            logger.setConsoleOutput(true);
        }
        
        std::string input_path = config.get<std::string>("input");
        std::string output_dir = config.get<std::string>("output", "./output");
        int num_threads = config.get<int>("threads", 4);
        std::string processor_type = config.get<std::string>("type", "auto");
        bool show_stats = config.get<bool>("stats", false);
        bool verbose = config.get<bool>("verbose", false);
        
        logger.info("Starting file processing system");
        logger.info("Input: " + input_path);
        logger.info("Output: " + output_dir);
        logger.info("Threads: " + std::to_string(num_threads));
        
        fs::create_directories(output_dir);
        
        std::vector<std::string> files = collect_files(input_path);
        if (files.empty()) {
            logger.error("No files found to process");
            return 1;
        }
        
        logger.info("Found " + std::to_string(files.size()) + " files to process");
        
        size_t total_size = calculate_total_size(files);
        
        auto progress_monitor = std::make_shared<ProgressMonitor>(verbose);
        progress_monitor->set_totals(files.size(), total_size);
        
        ThreadPool thread_pool(num_threads);
        ProcessingStats stats;
        
        Timer total_timer;
        total_timer.start();
        
        std::vector<std::future<ProcessResult>> futures;
        
        for (const auto& file : files) {
            auto processor = create_processor(processor_type, output_dir);
            processor->attach_progress_observer(progress_monitor);
            
            auto future = thread_pool.enqueue([processor = std::move(processor), file]() {
                return processor->process(file);
            });
            
            futures.push_back(std::move(future));
        }
        
        for (auto& future : futures) {
            try {
                ProcessResult result = future.get();
                stats.files_processed++;
                stats.bytes_processed += result.bytes_processed;
                
                if (!result.success) {
                    stats.errors++;
                    logger.error("Processing failed: " + result.message);
                }
            } catch (const std::exception& e) {
                stats.errors++;
                logger.error("Task execution failed: " + std::string(e.what()));
            }
        }
        
        total_timer.stop();
        stats.end_time = std::chrono::steady_clock::now();
        
        progress_monitor->print_summary();
        
        if (show_stats) {
            std::cout << "\n=== Performance Statistics ===\n";
            std::cout << "Total files: " << files.size() << "\n";
            std::cout << "Successfully processed: " << stats.files_processed.load() << "\n";
            std::cout << "Errors: " << stats.errors.load() << "\n";
            std::cout << "Total bytes: " << total_size << "\n";
            std::cout << "Processing time: " << total_timer.elapsed_seconds() << " seconds\n";
            std::cout << "Throughput: " << stats.get_throughput_mbps() << " MB/s\n";
            std::cout << "Threads used: " << num_threads << "\n";
            std::cout << "===============================\n";
        }
        
        logger.info("File processing completed");
        
        return stats.errors.load() > 0 ? 1 : 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return 1;
    }
}