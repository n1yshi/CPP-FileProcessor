#include "../src/core/ThreadPool.h"
#include "../src/utils/Logger.h"
#include <cassert>
#include <chrono>

void test_basic_functionality() {
    std::cout << "Testing ThreadPool basic functionality...\n";
    
    ThreadPool pool(4);
    
    auto result = pool.enqueue([]() {
        return 42;
    });
    
    assert(result.get() == 42);
    std::cout << "✓ Basic task execution works\n";
}

void test_multiple_tasks() {
    std::cout << "Testing multiple tasks...\n";
    
    ThreadPool pool(2);
    std::vector<std::future<int>> results;
    
    for (int i = 0; i < 10; ++i) {
        results.emplace_back(
            pool.enqueue([i]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                return i * i;
            })
        );
    }
    
    for (int i = 0; i < 10; ++i) {
        assert(results[i].get() == i * i);
    }
    
    std::cout << "✓ Multiple tasks executed correctly\n";
}

void test_exception_handling() {
    std::cout << "Testing exception handling...\n";
    
    ThreadPool pool(2);
    
    auto result = pool.enqueue([]() -> int {
        throw std::runtime_error("Test exception");
        return 0;
    });
    
    try {
        result.get();
        assert(false);
    } catch (const std::runtime_error& e) {
        assert(std::string(e.what()) == "Test exception");
    }
    
    std::cout << "✓ Exception handling works\n";
}

void test_wait_for_all() {
    std::cout << "Testing wait_for_all...\n";
    
    ThreadPool pool(2);
    std::atomic<int> counter{0};
    
    for (int i = 0; i < 5; ++i) {
        pool.enqueue([&counter]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            counter++;
        });
    }
    
    pool.wait_for_all();
    assert(counter.load() == 5);
    
    std::cout << "✓ wait_for_all works correctly\n";
}

void test_thread_safety() {
    std::cout << "Testing thread safety...\n";
    
    ThreadPool pool(4);
    std::atomic<int> shared_counter{0};
    std::vector<std::future<void>> futures;
    
    for (int i = 0; i < 100; ++i) {
        futures.emplace_back(
            pool.enqueue([&shared_counter]() {
                for (int j = 0; j < 100; ++j) {
                    shared_counter++;
                }
            })
        );
    }
    
    for (auto& future : futures) {
        future.get();
    }
    
    assert(shared_counter.load() == 10000);
    std::cout << "✓ Thread safety maintained\n";
}

void benchmark_performance() {
    std::cout << "Benchmarking ThreadPool performance...\n";
    
    const int num_tasks = 1000;
    const int num_threads = 4;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    {
        ThreadPool pool(num_threads);
        std::vector<std::future<int>> futures;
        
        for (int i = 0; i < num_tasks; ++i) {
            futures.emplace_back(
                pool.enqueue([i]() {
                    int sum = 0;
                    for (int j = 0; j < 1000; ++j) {
                        sum += j;
                    }
                    return sum + i;
                })
            );
        }
        
        for (auto& future : futures) {
            future.get();
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "✓ Processed " << num_tasks << " tasks in " << duration.count() 
              << "ms using " << num_threads << " threads\n";
    std::cout << "✓ Average: " << (duration.count() / static_cast<double>(num_tasks)) 
              << "ms per task\n";
}

int main() {
    std::cout << "=== ThreadPool Test Suite ===\n\n";
    
    try {
        test_basic_functionality();
        test_multiple_tasks();
        test_exception_handling();
        test_wait_for_all();
        test_thread_safety();
        benchmark_performance();
        
        std::cout << "\n✅ All ThreadPool tests passed!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Unknown test failure" << std::endl;
        return 1;
    }
}