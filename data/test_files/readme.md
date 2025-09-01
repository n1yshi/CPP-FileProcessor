# File Processing System Test Data

This directory contains various test files to demonstrate the capabilities of the multi-threaded file processing system.

## File Types

### Text Files
- **document1.txt**: Technical documentation with structured content
- **readme.md**: Markdown file with formatting and structure
- **log_sample.log**: Application log file with timestamps and structured entries

### Data Files
- **data.csv**: CSV file with user activity data and statistics
- **config_example.json**: Complex JSON configuration file

## Processing Examples

### Basic Text Processing
```bash
./file_processor -i data/test_files/document1.txt -v
```

### Batch Processing
```bash
./file_processor -i data/test_files/ -o results/ -t 4 -s
```

### Large File Processing
```bash
./file_processor -i data/test_files/data.csv --type text -v
```

## Expected Outputs

The system will generate analysis files for each processed document:

- **Word frequency analysis**: Most common words and their counts
- **Statistical information**: Line count, word count, character count
- **Processing metadata**: Processing time, file size, thread information

## Performance Characteristics

### Small Files (< 1KB)
- Processing time: 1-5ms
- Memory usage: Minimal
- Thread overhead: Low

### Medium Files (1KB - 1MB)
- Processing time: 10-100ms
- Memory usage: Proportional to file size
- Optimal for parallel processing

### Large Files (> 1MB)
- Processing time: 100ms+
- Memory usage: Chunked processing
- Benefits from multi-threading

## Test Scenarios

### Functionality Tests
1. Single file processing
2. Directory batch processing
3. Mixed file type handling
4. Error recovery and reporting

### Performance Tests
1. Throughput measurement
2. Memory usage monitoring
3. Thread utilization analysis
4. Scalability testing

### Stress Tests
1. High file count processing
2. Large file handling
3. Concurrent access patterns
4. Resource exhaustion scenarios

## Validation

After processing, verify:
- All input files are processed
- Output files are generated correctly
- Analysis reports contain expected data
- No memory leaks or resource issues
- Performance metrics are reasonable

## Troubleshooting

Common issues and solutions:

### File Access Errors
- Check file permissions
- Verify input path exists
- Ensure output directory is writable

### Performance Issues
- Adjust thread count based on CPU cores
- Monitor memory usage
- Check disk I/O performance

### Processing Errors
- Validate file encoding
- Check file format compatibility
- Review error logs for details