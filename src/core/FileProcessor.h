#pragma once

#include "../../include/common.h"
#include "../observers/Observer.h"

class IFileProcessor {
public:
    virtual ~IFileProcessor() = default;
    virtual ProcessResult process(const std::string& filepath) = 0;
    virtual bool canProcess(const std::string& extension) const = 0;
    virtual std::string getProcessorName() const = 0;
};

template<typename Derived>
class FileProcessor : public IFileProcessor {
protected:
    Subject<ProgressEvent> progress_subject_;
    std::string output_directory_;
    
public:
    explicit FileProcessor(const std::string& output_dir = "./output") 
        : output_directory_(output_dir) {
        fs::create_directories(output_directory_);
    }
    
    void attach_progress_observer(std::shared_ptr<Observer<ProgressEvent>> observer) {
        progress_subject_.attach(observer);
    }
    
    void detach_progress_observer(std::shared_ptr<Observer<ProgressEvent>> observer) {
        progress_subject_.detach(observer);
    }
    
    ProcessResult process(const std::string& filepath) override {
        ProcessResult result;
        Timer timer;
        timer.start();
        
        try {
            if (!fs::exists(filepath)) {
                result.message = "File does not exist: " + filepath;
                return result;
            }
            
            size_t file_size = fs::file_size(filepath);
            progress_subject_.notify_all(ProgressEvent(filepath, 0, file_size, "started"));
            
            result = static_cast<Derived*>(this)->process_impl(filepath);
            result.bytes_processed = file_size;
            
            timer.stop();
            result.processing_time = timer.elapsed_milliseconds();
            
            std::string status = result.success ? "completed" : "failed";
            progress_subject_.notify_all(ProgressEvent(filepath, file_size, file_size, status));
            
        } catch (const std::exception& e) {
            result.success = false;
            result.message = "Processing failed: " + std::string(e.what());
            timer.stop();
            result.processing_time = timer.elapsed_milliseconds();
        }
        
        return result;
    }
    
protected:
    void notify_progress(const std::string& filepath, size_t processed, size_t total, const std::string& status) {
        progress_subject_.notify_all(ProgressEvent(filepath, processed, total, status));
    }
    
    std::string get_output_path(const std::string& input_path, const std::string& suffix = "") const {
        fs::path input(input_path);
        std::string filename = input.stem().string() + suffix + input.extension().string();
        return (fs::path(output_directory_) / filename).string();
    }
};