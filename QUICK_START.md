# Quick Start Guide - Running the Code Locally

## Prerequisites

- **C++17 compatible compiler** (g++ on Linux, clang++ on macOS)
- **Make utility** (usually pre-installed)
- **OpenMP** (optional, for parallel speedup - usually included with g++)

## Step-by-Step Instructions

### 1. Navigate to Project Directory

```bash
cd /path/to/finalproject
```

### 2. Build the Project

**Option A: Build with Make (Recommended)**
```bash
make
```

**Option B: Build without OpenMP (if OpenMP not available)**
```bash
make no-openmp
```

**Option C: Manual Build**
```bash
g++ -std=c++17 -Wall -Wextra -O3 -fopenmp -I./include \
    src/TechnicalIndicator.cpp src/Scheduler.cpp src/main.cpp src/StockDataFetcher.cpp \
    -o stock_analyzer -fopenmp
```

### 3. Run the Program

**Basic Usage (Benchmark Mode):**
```bash
./stock_analyzer
```
This runs with 10 stocks by default and shows performance comparison.

**Run with Specific Number of Stocks:**
```bash
./stock_analyzer 20
```

**Run with Scheduler Mode (Real-time Analysis):**
```bash
./stock_analyzer 10 scheduler
```

**Run Large Scale Test:**
```bash
./stock_analyzer 100
```

## Common Commands

### Clean Build Artifacts
```bash
make clean
```

### Rebuild from Scratch
```bash
make clean && make
```

### Check if Executable Exists
```bash
ls -lh stock_analyzer
```

## Troubleshooting

### Issue: "fopenmp: command not found" (macOS)

**Solution:**
```bash
# Install OpenMP
brew install libomp

# Or build without OpenMP
make no-openmp
```

### Issue: "Makefile: No such file or directory"

**Solution:**
```bash
# Ensure you're in the project directory
pwd
# Should show: .../finalproject

# Check if Makefile exists
ls Makefile
```

### Issue: Permission Denied

**Solution:**
```bash
chmod +x stock_analyzer
```

### Issue: Compilation Errors

**Solution:**
```bash
# Check compiler version
g++ --version
# Should support C++17

# Try building without OpenMP first
make no-openmp
```

## Example Output

When you run `./stock_analyzer 10`, you should see:

```
=== Parallel Stock Market Analysis System ===

OpenMP enabled with X threads
Number of stocks: 10
Run scheduler: No

Generating sample stock data...
Generated data for 10 stocks

=== Performance Benchmark ===
Running sequential computation...
Sequential time: X.XXX seconds
Running parallel computation...
Parallel time: X.XXX seconds

=== Performance Summary ===
Sequential time: X.XXX seconds
Parallel time: X.XXX seconds
Speedup: X.XXx
Efficiency: XX.XX%
Results match: Yes

=== Analysis Results ===
Symbol    Signal    Strength    RSI       SMA20     SMA50     
------------------------------------------------------------------------
AAPL      BUY       3.45        65.23     150.25    148.50    
...
```

## Quick Test

To verify everything works:
```bash
make clean
make no-openmp
./stock_analyzer 5
```

You should see analysis results without errors.

