#include "TextProcessor.h"
#include "../utils/Logger.h"

TextProcessor::TextProcessor(const std::string& output_dir, size_t chunk_size)
    : FileProcessor(output_dir), chunk_size_(chunk_size) {}

ProcessResult TextProcessor::process_impl(const std::string& filepath) {
    ProcessResult result;
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        result.message = "Cannot open file: " + filepath;
        return result;
    }
    
    std::string content;
    std::string line;
    size_t processed_bytes = 0;
    size_t total_bytes = fs::file_size(filepath);
    
    while (std::getline(file, line)) {
        content += line + "\n";
        processed_bytes += line.length() + 1;
        
        if (content.length() >= chunk_size_) {
            std::string processed_chunk = process_chunk(content);
            content.clear();
            
            notify_progress(filepath, processed_bytes, total_bytes, "processing");
        }
    }
    
    if (!content.empty()) {
        process_chunk(content);
    }
    
    file.close();
    
    std::ifstream full_file(filepath);
    std::string full_content((std::istreambuf_iterator<char>(full_file)),
                            std::istreambuf_iterator<char>());
    full_file.close();
    
    TextStats stats = analyze_text(full_content);
    
    std::string output_path = get_output_path(filepath, "_analysis");
    write_analysis_report(output_path, stats);
    
    result.success = true;
    result.message = "Text processing completed";
    result.metadata["lines"] = std::to_string(stats.lines);
    result.metadata["words"] = std::to_string(stats.words);
    result.metadata["characters"] = std::to_string(stats.characters);
    result.metadata["output_file"] = output_path;
    
    return result;
}

bool TextProcessor::canProcess(const std::string& extension) const {
    static const std::unordered_set<std::string> supported_extensions = {
        ".txt", ".md", ".csv", ".log", ".json", ".xml", ".html", ".css", ".js"
    };
    return supported_extensions.find(extension) != supported_extensions.end();
}

std::string TextProcessor::getProcessorName() const {
    return "TextProcessor";
}

TextProcessor::TextStats TextProcessor::analyze_text(const std::string& content) {
    TextStats stats;
    
    stats.characters = content.length();
    
    std::istringstream stream(content);
    std::string line;
    bool in_paragraph = false;
    
    while (std::getline(stream, line)) {
        stats.lines++;
        
        if (line.empty()) {
            if (in_paragraph) {
                stats.paragraphs++;
                in_paragraph = false;
            }
        } else {
            in_paragraph = true;
            
            std::vector<std::string> words = tokenize(line);
            stats.words += words.size();
            
            for (const auto& word : words) {
                std::string lower_word = to_lower(word);
                stats.word_frequency[lower_word]++;
            }
        }
    }
    
    if (in_paragraph) {
        stats.paragraphs++;
    }
    
    return stats;
}

std::string TextProcessor::process_chunk(const std::string& chunk) {
    std::string processed = chunk;
    
    std::transform(processed.begin(), processed.end(), processed.begin(),
                  [](char c) { return std::isspace(c) ? ' ' : c; });
    
    processed.erase(std::unique(processed.begin(), processed.end(),
                               [](char a, char b) { return a == ' ' && b == ' '; }),
                   processed.end());
    
    return processed;
}

std::vector<std::string> TextProcessor::tokenize(const std::string& text) {
    std::vector<std::string> tokens;
    std::string current_word;
    
    for (char c : text) {
        if (is_word_char(c)) {
            current_word += c;
        } else {
            if (!current_word.empty()) {
                tokens.push_back(current_word);
                current_word.clear();
            }
        }
    }
    
    if (!current_word.empty()) {
        tokens.push_back(current_word);
    }
    
    return tokens;
}

std::string TextProcessor::to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool TextProcessor::is_word_char(char c) {
    return std::isalnum(c) || c == '_' || c == '-';
}

void TextProcessor::write_analysis_report(const std::string& output_path, const TextStats& stats) {
    std::ofstream report(output_path);
    if (!report.is_open()) {
        Logger::getInstance().error("Cannot create analysis report: " + output_path);
        return;
    }
    
    report << "Text Analysis Report\n";
    report << "===================\n\n";
    report << "Statistics:\n";
    report << "  Lines: " << stats.lines << "\n";
    report << "  Words: " << stats.words << "\n";
    report << "  Characters: " << stats.characters << "\n";
    report << "  Paragraphs: " << stats.paragraphs << "\n\n";
    
    report << "Top 10 Most Frequent Words:\n";
    
    std::vector<std::pair<std::string, size_t>> word_pairs(
        stats.word_frequency.begin(), stats.word_frequency.end());
    
    std::sort(word_pairs.begin(), word_pairs.end(),
             [](const auto& a, const auto& b) { return a.second > b.second; });
    
    for (size_t i = 0; i < std::min(size_t(10), word_pairs.size()); ++i) {
        report << "  " << (i + 1) << ". " << word_pairs[i].first 
               << " (" << word_pairs[i].second << " times)\n";
    }
    
    report.close();
}