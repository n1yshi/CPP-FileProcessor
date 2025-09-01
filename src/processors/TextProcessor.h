#pragma once

#include "../core/FileProcessor.h"

class TextProcessor : public FileProcessor<TextProcessor> {
private:
    size_t chunk_size_;
    
public:
    explicit TextProcessor(const std::string& output_dir = "./output", size_t chunk_size = 1024);
    
    ProcessResult process_impl(const std::string& filepath);
    bool canProcess(const std::string& extension) const override;
    std::string getProcessorName() const override;
    
private:
    struct TextStats {
        size_t lines = 0;
        size_t words = 0;
        size_t characters = 0;
        size_t paragraphs = 0;
        std::unordered_map<std::string, size_t> word_frequency;
    };
    
    TextStats analyze_text(const std::string& content);
    std::string process_chunk(const std::string& chunk);
    std::vector<std::string> tokenize(const std::string& text);
    std::string to_lower(const std::string& str);
    bool is_word_char(char c);
    void write_analysis_report(const std::string& output_path, const TextStats& stats);
};