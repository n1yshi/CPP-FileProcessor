# Multi-threaded File Processing System

A sophisticated C++ application that demonstrates advanced programming concepts including multi-threading, design patterns, template programming, and modern C++ features.

## Features

- Multi-threaded file processing with thread pool
- Plugin-based architecture for different file processors
- Observer pattern for progress monitoring
- Factory pattern for processor creation
- Template-based generic data structures
- Memory management with smart pointers
- Exception handling and logging
- Configuration management
- Performance monitoring and statistics

## Architecture

The system uses several design patterns:

- **Factory Pattern**: Creates different types of file processors
- **Observer Pattern**: Notifies about processing progress
- **Strategy Pattern**: Different processing algorithms
- **Singleton Pattern**: Configuration and logger management
- **Template Pattern**: Generic data structures and algorithms

## Project Structure

```
cpp-file-processor/
├── src/
│   ├── core/
│   │   ├── FileProcessor.h
│   │   ├── FileProcessor.cpp
│   │   ├── ThreadPool.h
│   │   ├── ThreadPool.cpp
│   │   └── TaskQueue.h
│   ├── processors/
│   │   ├── TextProcessor.h
│   │   ├── TextProcessor.cpp
│   │   ├── ImageProcessor.h
│   │   ├── ImageProcessor.cpp
│   │   └── ProcessorFactory.h
│   ├── observers/
│   │   ├── Observer.h
│   │   ├── ProgressMonitor.h
│   │   └── ProgressMonitor.cpp
│   ├── utils/
│   │   ├── Logger.h
│   │   ├── Logger.cpp
│   │   ├── Config.h
│   │   ├── Config.cpp
│   │   └── Timer.h
│   └── main.cpp
├── include/
│   └── common.h
├── tests/
│   ├── test_threadpool.cpp
│   ├── test_processors.cpp
│   └── test_main.cpp
├── data/
│   ├── sample.txt
│   ├── config.json
│   └── test_files/
├── Makefile
└── README.md
```

## Requirements

- C++17 or later
- GCC 7.0+ or Clang 5.0+
- Make
- pthread library
- Optional: Google Test for unit tests

## Compilation

### Using Make
```bash
make all          # Build everything
make release      # Build optimized version
make debug        # Build debug version
make test         # Build and run tests
make clean        # Clean build files
```

### Manual Compilation
```bash
g++ -std=c++17 -pthread -O2 -Wall -Wextra \
    src/main.cpp \
    src/core/*.cpp \
    src/processors/*.cpp \
    src/observers/*.cpp \
    src/utils/*.cpp \
    -I include \
    -o file_processor
```

## Usage

### Basic Usage
```bash
./file_processor --input data/sample.txt --threads 4 --type text
```

### Advanced Usage
```bash
./file_processor \
    --input data/test_files/ \
    --output results/ \
    --threads 8 \
    --type auto \
    --config data/config.json \
    --verbose \
    --stats
```

### Command Line Options
- `--input, -i`: Input file or directory
- `--output, -o`: Output directory (default: ./output)
- `--threads, -t`: Number of worker threads (default: 4)
- `--type`: Processor type (text, image, auto)
- `--config, -c`: Configuration file path
- `--verbose, -v`: Enable verbose logging
- `--stats, -s`: Show performance statistics
- `--help, -h`: Show help message

## Configuration

The system uses a JSON configuration file:

```json
{
  "processing": {
    "max_threads": 8,
    "queue_size": 100,
    "timeout_ms": 5000
  },
  "logging": {
    "level": "INFO",
    "file": "processor.log",
    "console": true
  },
  "processors": {
    "text": {
      "chunk_size": 1024,
      "encoding": "utf-8"
    },
    "image": {
      "max_width": 1920,
      "max_height": 1080,
      "quality": 85
    }
  }
}
```

## Key Components

### ThreadPool
Manages a pool of worker threads for concurrent file processing.

### FileProcessor
Abstract base class for different file processors with template support.

### ProcessorFactory
Creates appropriate processors based on file type using factory pattern.

### Observer System
Monitors processing progress and provides real-time feedback.

### Logger
Thread-safe logging system with different log levels.

## Performance Features

- Lock-free queue implementation for high throughput
- Memory pool for reduced allocation overhead
- SIMD optimizations for data processing
- Cache-friendly data structures
- Profiling and benchmarking tools

## Testing

Run the test suite:
```bash
make test
```

Individual test categories:
```bash
./tests/test_threadpool
./tests/test_processors
```

## Examples

### Processing Text Files
The system can count words, analyze sentiment, extract keywords, and perform text transformations.

### Processing Images
Supports resizing, format conversion, metadata extraction, and basic image analysis.

### Batch Processing
Efficiently processes entire directories with automatic file type detection.

## Advanced Features

### Custom Processors
Implement the IFileProcessor interface to create custom processors:

```cpp
class CustomProcessor : public IFileProcessor {
public:
    ProcessResult process(const std::string& filepath) override;
    bool canProcess(const std::string& extension) override;
};
```

### Plugin System
Load processors dynamically at runtime using shared libraries.

### Memory Management
Uses RAII principles with smart pointers and automatic resource management.

## Performance Benchmarks

Typical performance on modern hardware:
- Text processing: 50-100 MB/s per thread
- Image processing: 10-20 images/s per thread
- Memory usage: 50-200 MB depending on file sizes
- Thread overhead: <1% for files >1MB

## Troubleshooting

### Common Issues
1. **Compilation errors**: Ensure C++17 support
2. **Threading issues**: Check pthread library linking
3. **Memory errors**: Run with valgrind for debugging
4. **Performance**: Use profiler to identify bottlenecks

### Debug Mode
Compile with debug flags for detailed error information:
```bash
make debug
gdb ./file_processor
```

## Contributing

1. Follow C++ Core Guidelines
2. Use consistent code formatting
3. Add unit tests for new features
4. Update documentation
5. Profile performance impact

## License

MIT License - see LICENSE file for details