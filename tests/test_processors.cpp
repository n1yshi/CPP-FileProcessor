#include "../src/processors/TextProcessor.h"
#include "../src/observers/ProgressMonitor.h"
#include "../src/utils/Logger.h"
#include <cassert>
#include <fstream>

void create_test_file(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

void test_text_processor_basic() {
    std::cout << "Testing TextProcessor basic functionality...\n";
    
    const std::string test_content = 
        "Hello world! This is a test file.\n"
        "It contains multiple lines and words.\n"
        "We will test the text processing capabilities.\n"
        "\n"
        "This is a new paragraph with more text.\n"
        "The quick brown fox jumps over the lazy dog.\n";
    
    create_test_file("test_input.txt", test_content);
    
    TextProcessor processor("./test_output");
    ProcessResult result = processor.process("test_input.txt");
    
    assert(result.success);
    assert(result.bytes_processed > 0);
    assert(result.processing_time.count() >= 0);
    
    std::cout << "✓ Basic text processing works\n";
    std::cout << "  - Processed " << result.bytes_processed << " bytes\n";
    std::cout << "  - Processing time: " << result.processing_time.count() << "ms\n";
    
    fs::remove("test_input.txt");
    fs::remove_all("./test_output");
}

void test_text_processor_with_observer() {
    std::cout << "Testing TextProcessor with progress observer...\n";
    
    const std::string test_content = 
        "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
        "Sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n"
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco.\n"
        "Laboris nisi ut aliquip ex ea commodo consequat.\n"
        "Duis aute irure dolor in reprehenderit in voluptate velit esse.\n"
        "Cillum dolore eu fugiat nulla pariatur.\n"
        "Excepteur sint occaecat cupidatat non proident.\n"
        "Sunt in culpa qui officia deserunt mollit anim id est laborum.\n";
    
    create_test_file("test_observer.txt", test_content);
    
    TextProcessor processor("./test_output");
    auto monitor = std::make_shared<ProgressMonitor>(true);
    processor.attach_progress_observer(monitor);
    
    ProcessResult result = processor.process("test_observer.txt");
    
    assert(result.success);
    assert(!result.metadata.empty());
    assert(result.metadata.find("lines") != result.metadata.end());
    assert(result.metadata.find("words") != result.metadata.end());
    
    std::cout << "✓ Progress observer integration works\n";
    std::cout << "  - Lines: " << result.metadata["lines"] << "\n";
    std::cout << "  - Words: " << result.metadata["words"] << "\n";
    std::cout << "  - Characters: " << result.metadata["characters"] << "\n";
    
    fs::remove("test_observer.txt");
    fs::remove_all("./test_output");
}

void test_file_extension_support() {
    std::cout << "Testing file extension support...\n";
    
    TextProcessor processor;
    
    assert(processor.canProcess(".txt"));
    assert(processor.canProcess(".md"));
    assert(processor.canProcess(".json"));
    assert(processor.canProcess(".csv"));
    assert(processor.canProcess(".log"));
    assert(!processor.canProcess(".exe"));
    assert(!processor.canProcess(".bin"));
    
    std::cout << "✓ File extension detection works\n";
}

void test_large_file_processing() {
    std::cout << "Testing large file processing...\n";
    
    std::string large_content;
    for (int i = 0; i < 1000; ++i) {
        large_content += "This is line " + std::to_string(i) + " of a large test file. ";
        large_content += "It contains repeated content to test chunked processing. ";
        large_content += "The processor should handle this efficiently.\n";
    }
    
    create_test_file("large_test.txt", large_content);
    
    TextProcessor processor("./test_output", 512);
    
    auto start = std::chrono::high_resolution_clock::now();
    ProcessResult result = processor.process("large_test.txt");
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    assert(result.success);
    assert(result.bytes_processed > 50000);
    
    std::cout << "✓ Large file processing works\n";
    std::cout << "  - File size: " << result.bytes_processed << " bytes\n";
    std::cout << "  - Processing time: " << duration.count() << "ms\n";
    std::cout << "  - Throughput: " << (result.bytes_processed / 1024.0 / 1024.0) / (duration.count() / 1000.0) << " MB/s\n";
    
    fs::remove("large_test.txt");
    fs::remove_all("./test_output");
}

void test_error_handling() {
    std::cout << "Testing error handling...\n";
    
    TextProcessor processor("./test_output");
    
    ProcessResult result = processor.process("nonexistent_file.txt");
    assert(!result.success);
    assert(!result.message.empty());
    
    std::cout << "✓ Error handling works\n";
    std::cout << "  - Error message: " << result.message << "\n";
}

void test_json_processing() {
    std::cout << "Testing JSON file processing...\n";
    
    const std::string json_content = R"({
    "name": "Test Configuration",
    "version": "1.0.0",
    "settings": {
        "debug": true,
        "max_connections": 100,
        "timeout": 5000
    },
    "features": [
        "authentication",
        "logging",
        "monitoring"
    ],
    "database": {
        "host": "localhost",
        "port": 5432,
        "name": "testdb"
    }
})";
    
    create_test_file("test_config.json", json_content);
    
    TextProcessor processor("./test_output");
    ProcessResult result = processor.process("test_config.json");
    
    assert(result.success);
    assert(processor.canProcess(".json"));
    
    std::cout << "✓ JSON processing works\n";
    
    fs::remove("test_config.json");
    fs::remove_all("./test_output");
}

void benchmark_text_processing() {
    std::cout << "Benchmarking text processing performance...\n";
    
    std::vector<std::string> test_files;
    
    for (int i = 0; i < 10; ++i) {
        std::string filename = "benchmark_" + std::to_string(i) + ".txt";
        std::string content;
        
        for (int j = 0; j < 100; ++j) {
            content += "Benchmark test file " + std::to_string(i) + " line " + std::to_string(j) + ". ";
            content += "This file is used for performance testing of the text processor. ";
            content += "It contains various words and sentences to analyze. ";
            content += "The quick brown fox jumps over the lazy dog. ";
            content += "Lorem ipsum dolor sit amet consectetur adipiscing elit.\n";
        }
        
        create_test_file(filename, content);
        test_files.push_back(filename);
    }
    
    TextProcessor processor("./test_output");
    auto start = std::chrono::high_resolution_clock::now();
    
    size_t total_bytes = 0;
    for (const auto& filename : test_files) {
        ProcessResult result = processor.process(filename);
        assert(result.success);
        total_bytes += result.bytes_processed;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    double throughput = (total_bytes / 1024.0 / 1024.0) / (duration.count() / 1000.0);
    
    std::cout << "✓ Benchmark completed\n";
    std::cout << "  - Files processed: " << test_files.size() << "\n";
    std::cout << "  - Total bytes: " << total_bytes << "\n";
    std::cout << "  - Total time: " << duration.count() << "ms\n";
    std::cout << "  - Throughput: " << throughput << " MB/s\n";
    
    for (const auto& filename : test_files) {
        fs::remove(filename);
    }
    fs::remove_all("./test_output");
}

int main() {
    std::cout << "=== File Processor Test Suite ===\n\n";
    
    try {
        test_text_processor_basic();
        test_text_processor_with_observer();
        test_file_extension_support();
        test_large_file_processing();
        test_error_handling();
        test_json_processing();
        benchmark_text_processing();
        
        std::cout << "\n✅ All processor tests passed!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown test failure" << std::endl;
        return 1;
    }
}