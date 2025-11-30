# Local Build Commands

## Quick Start

### 1. Navigate to Project Directory
```bash
cd /Users/shreyashidutta/Desktop/ECE6122/finalproject
```

### 2. Build the Project

**Option A: Build with OpenMP (Recommended)**
```bash
make clean
make
```

**Option B: Build without OpenMP (if OpenMP fails)**
```bash
make clean
make no-openmp
```

### 3. Run the Program

**Basic run (10 stocks, benchmark mode):**
```bash
./stock_analyzer
```

**Analyze specific number of stocks:**
```bash
./stock_analyzer 20
./stock_analyzer 50
./stock_analyzer 100
```

**Real-time scheduler mode:**
```bash
./stock_analyzer 10 scheduler
```

---

## Complete Build Sequence

```bash
# Step 1: Go to project directory
cd /Users/shreyashidutta/Desktop/ECE6122/finalproject

# Step 2: Clean previous builds
make clean

# Step 3: Build with OpenMP
make

# Step 4: Run
./stock_analyzer 10
```

---

## Alternative: Manual Build (if Make doesn't work)

```bash
cd /Users/shreyashidutta/Desktop/ECE6122/finalproject

# Build with OpenMP
clang++ -std=c++17 -Wall -Wextra -O3 -Xpreprocessor -fopenmp \
    -I/opt/homebrew/opt/libomp/include -I./include \
    src/TechnicalIndicator.cpp src/Scheduler.cpp src/main.cpp src/StockDataFetcher.cpp \
    -o stock_analyzer -L/opt/homebrew/opt/libomp/lib -lomp

# Or build without OpenMP
clang++ -std=c++17 -Wall -Wextra -O3 -I./include \
    src/TechnicalIndicator.cpp src/Scheduler.cpp src/main.cpp src/StockDataFetcher.cpp \
    -o stock_analyzer
```

---

## Common Commands

```bash
# Clean build artifacts
make clean

# Rebuild from scratch
make clean && make

# Check if executable exists
ls -lh stock_analyzer

# Run with different options
./stock_analyzer 5              # 5 stocks
./stock_analyzer 25             # 25 stocks
./stock_analyzer 10 scheduler   # Real-time mode
./stock_analyzer 100            # Large scale test
```

---

## Troubleshooting

**If OpenMP build fails:**
```bash
make no-openmp
```

**If permission denied:**
```bash
chmod +x stock_analyzer
```

**To see all Makefile targets:**
```bash
make help
```


