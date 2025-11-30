# Exact Commands to Run the Project

## Prerequisites
Make sure you're in the project directory:
```bash
cd /Users/shreyashidutta/Desktop/ECE6122/finalproject
```

## 1. Build the Project

### Clean and build everything:
```bash
make clean && make
```

### Build without OpenMP (if OpenMP fails):
```bash
make clean && make no-openmp
```

## 2. Run Unit Tests

```bash
make test
```

Or manually:
```bash
make clean
make test
./test_analyzer
```

## 3. Run Main Program

### Basic benchmark (10 stocks, default):
```bash
./stock_analyzer
```

### Benchmark with specific number of stocks:
```bash
./stock_analyzer 20
./stock_analyzer 50
./stock_analyzer 100
```

### Scheduler mode (hourly cycles):
```bash
./stock_analyzer 10 scheduler
```

### Scheduler mode without benchmark:
```bash
./stock_analyzer 10 scheduler no-benchmark
```

## 4. Using Makefile Targets

### Quick run (10 stocks):
```bash
make run
```

### Run with scheduler:
```bash
make run-scheduler
```

### Benchmark with 50 stocks:
```bash
make benchmark
```

### Large scale test (100 stocks):
```bash
make test-large
```

## 5. Complete Test Sequence

### Full test sequence:
```bash
# 1. Clean and build
make clean && make

# 2. Run unit tests
make test

# 3. Run benchmark with 20 stocks
./stock_analyzer 20

# 4. Run scheduler mode (will run for 1 hour, press Ctrl+C to stop)
./stock_analyzer 10 scheduler
```

## 6. Quick Verification

### Verify build succeeded:
```bash
ls -lh stock_analyzer
```

### Check if executable runs:
```bash
./stock_analyzer 5
```

## Expected Output

### Unit Tests:
```
=== Technical Indicator Unit Tests ===

Test 1: SMA Calculation... PASSED
Test 2: RSI Calculation... PASSED
Test 3: MACD Calculation... PASSED
Test 4: Signal Generation... PASSED
Test 5: Parallel vs Sequential Consistency... PASSED
Test 6: Edge Cases... PASSED

=== All Tests PASSED ===
```

### Benchmark Output:
- Performance Summary with visual charts
- Speedup visualization
- Efficiency charts
- Execution time comparison
- Sample analysis results

### Scheduler Mode:
- Starts scheduler with hourly cycles
- Runs data fetcher thread
- Runs notification dispatcher
- Press Ctrl+C to stop

