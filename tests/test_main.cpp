#include "../src/utils/Logger.h"
#include "../src/utils/Config.h"
#include "../src/utils/Timer.h"
#include "../src/observers/ProgressMonitor.h"
#include <cassert>
#include <fstream>

void test_logger() {
    std::cout << "Testing Logger functionality...\n";
    
    Logger& logger = Logger::getInstance();
    logger.setLevel(LogLevel::DEBUG);
    logger.setLogFile("test.log");
    
    logger.debug("Debug message");
    logger.info("Info message");
    logger.warning("Warning message");
    logger.error("Error message");
    
    std::ifstream log_file("test.log");
    assert(log_file.is_open());
    
    std::string line;
    int line_count = 0;
    while (std::getline(log_file, line)) {
        line_count++;
        assert(line.find("]") != std::string::npos);
    }
    log_file.close();
    
    assert(line_count >= 4);
    
    std::cout << "✓ Logger works correctly\n";
    fs::remove("test.log");
}

void test_config() {
    std::cout << "Testing Config functionality...\n";
    
    Config& config = Config::getInstance();
    
    config.set("test_string", "hello");
    config.set("test_int", "42");
    config.set("test_bool", "true");
    config.set("test_double", "3.14");
    
    assert(config.get<std::string>("test_string") == "hello");
    assert(config.get<int>("test_int") == 42);
    assert(config.get<bool>("test_bool") == true);
    assert(config.get<double>("test_double") == 3.14);
    
    assert(config.get<std::string>("nonexistent", "default") == "default");
    assert(config.get<int>("nonexistent", 100) == 100);
    
    assert(config.has("test_string"));
    assert(!config.has("nonexistent"));
    
    std::cout << "✓ Config works correctly\n";
}

void test_timer() {
    std::cout << "Testing Timer functionality...\n";
    
    Timer timer;
    timer.start();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    timer.stop();
    
    assert(timer.elapsed_milliseconds().count() >= 100);
    assert(timer.elapsed_seconds() >= 0.1);
    assert(!timer.is_running());
    
    timer.reset();
    assert(timer.elapsed_seconds() == 0.0);
    
    std::cout << "✓ Timer works correctly\n";
}

void test_scoped_timer() {
    std::cout << "Testing ScopedTimer functionality...\n";
    
    Timer timer;
    
    {
        ScopedTimer scoped(timer);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    assert(timer.elapsed_milliseconds().count() >= 50);
    assert(!timer.is_running());
    
    std::cout << "✓ ScopedTimer works correctly\n";
}

void test_progress_monitor() {
    std::cout << "Testing ProgressMonitor functionality...\n";
    
    ProgressMonitor monitor(false);
    monitor.set_totals(3, 300);
    
    ProgressEvent event1("file1.txt", 100, 100, "completed");
    ProgressEvent event2("file2.txt", 100, 100, "completed");
    ProgressEvent event3("file3.txt", 100, 100, "completed");
    
    monitor.notify(event1);
    monitor.notify(event2);
    monitor.notify(event3);
    
    std::cout << "✓ ProgressMonitor works correctly\n";
}

void test_thread_safe_queue() {
    std::cout << "Testing ThreadSafeQueue functionality...\n";
    
    ThreadSafeQueue<int> queue;
    
    assert(queue.empty());
    assert(queue.size() == 0);
    
    queue.push(1);
    queue.push(2);
    queue.push(3);
    
    assert(!queue.empty());
    assert(queue.size() == 3);
    
    int item;
    assert(queue.try_pop(item));
    assert(item == 1);
    assert(queue.size() == 2);
    
    queue.wait_and_pop(item);
    assert(item == 2);
    assert(queue.size() == 1);
    
    std::cout << "✓ ThreadSafeQueue works correctly\n";
}

void test_concurrent_queue_access() {
    std::cout << "Testing concurrent queue access...\n";
    
    ThreadSafeQueue<int> queue;
    std::atomic<int> sum{0};
    std::vector<std::thread> threads;
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&queue, i]() {
            for (int j = 0; j < 100; ++j) {
                queue.push(i * 100 + j);
            }
        });
    }
    
    threads.emplace_back([&queue, &sum]() {
        int item;
        for (int i = 0; i < 400; ++i) {
            queue.wait_and_pop(item);
            sum += item;
        }
    });
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    int expected_sum = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 100; ++j) {
            expected_sum += i * 100 + j;
        }
    }
    
    assert(sum.load() == expected_sum);
    assert(queue.empty());
    
    std::cout << "✓ Concurrent queue access works correctly\n";
}

void test_processing_stats() {
    std::cout << "Testing ProcessingStats functionality...\n";
    
    ProcessingStats stats;
    
    stats.files_processed = 10;
    stats.bytes_processed = 1024 * 1024;
    stats.errors = 2;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    double duration = stats.get_duration_seconds();
    double throughput = stats.get_throughput_mbps();
    
    assert(duration >= 0.1);
    assert(throughput > 0);
    
    std::cout << "✓ ProcessingStats works correctly\n";
    std::cout << "  - Duration: " << duration << " seconds\n";
    std::cout << "  - Throughput: " << throughput << " MB/s\n";
}

void create_test_config_file() {
    std::ofstream config_file("test_config.conf");
    config_file << "# Test configuration file\n";
    config_file << "max_threads=8\n";
    config_file << "chunk_size=2048\n";
    config_file << "verbose=true\n";
    config_file << "output_dir=\"./test_output\"\n";
    config_file.close();
}

void test_config_file_loading() {
    std::cout << "Testing config file loading...\n";
    
    create_test_config_file();
    
    Config& config = Config::getInstance();
    bool loaded = config.loadFromFile("test_config.conf");
    
    assert(loaded);
    assert(config.get<int>("max_threads") == 8);
    assert(config.get<int>("chunk_size") == 2048);
    assert(config.get<bool>("verbose") == true);
    assert(config.get<std::string>("output_dir") == "./test_output");
    
    std::cout << "✓ Config file loading works correctly\n";
    
    fs::remove("test_config.conf");
}

int main() {
    std::cout << "=== Utility Components Test Suite ===\n\n";
    
    try {
        test_logger();
        test_config();
        test_timer();
        test_scoped_timer();
        test_progress_monitor();
        test_thread_safe_queue();
        test_concurrent_queue_access();
        test_processing_stats();
        test_config_file_loading();
        
        std::cout << "\n✅ All utility tests passed!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown test failure" << std::endl;
        return 1;
    }
}