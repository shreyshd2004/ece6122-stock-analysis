# Parallel Pathfinding for Stock Market Analysis

**ECE 6122 Final Project**

## Team Members

1. **Hersh Gupta** - Technical Indicator Computation & OpenMP Parallelization
2. **Shreyashi Dutta** - Scheduler & Thread Orchestration
3. **Tanmay Arvind** - System Integration & Performance Testing

---

## Project Overview

This project implements a high-performance, parallel stock market analysis system designed to generate real-time buy and sell signals efficiently. The system combines **data parallelism** (OpenMP) and **task parallelism** (std::threads) to process large portfolios of stocks with multiple technical indicators.

### Key Features

- **Data Parallelism (OpenMP)**: Parallelizes technical indicator calculations across multiple stocks using all available CPU cores
- **Task Parallelism (std::threads)**: Manages concurrent tasks including data fetching, scheduling, and notifications
- **Socket Programming**: Real-time stock data fetching using TCP/IP socket connections to financial APIs
- **Thread-Safe Communication**: Uses mutexes and condition variables for safe inter-thread communication
- **Real-Time Analysis**: Scheduler coordinates hourly analysis cycles with background data updates
- **Performance Optimization**: Achieves significant speedup (target: 30%+ improvement, ideally reducing minutes to seconds)

## Architecture

### Components

1. **TechnicalIndicator Class** (Hersh Gupta)
   - Encapsulates indicator computation logic (SMA, RSI, MACD)
   - OpenMP parallelization for multi-stock analysis
   - Signal generation (BUY/SELL/HOLD)

2. **Scheduler Class** (Shreyashi Dutta)
   - Manages recurring analysis intervals
   - Coordinates background threads (data fetcher, notification dispatcher)
   - Thread-safe queue for inter-thread communication

3. **StockDataFetcher Class** (Shreyashi Dutta)
   - Socket-based data fetching from financial APIs
   - TCP/IP socket connections for real-time market data
   - HTTP client implementation using POSIX sockets
   - Fallback to sample data if network unavailable

4. **Integration & Testing** (Tanmay Arvind)
   - Main application integration
   - Performance benchmarking and validation
   - Scalability testing

## Building the Project

### System Requirements

- **Operating System**: Linux (PACE-ICE compatible) or macOS
- **Compiler**: g++ (GCC) with C++17 support
- **OpenMP**: Included with g++ on Linux systems (standard)
- **Build System**: Make utility
- **Third-Party Libraries**: None required (uses only standard C++17 and OpenMP)

### Prerequisites for PACE-ICE

The project is designed to compile and run on PACE-ICE Linux systems. No additional libraries or dependencies are required beyond:
- Standard C++17 compiler (g++ typically available)
- OpenMP support (included with g++)

### Build Instructions

#### On PACE-ICE (Linux):

```bash
# Navigate to project directory
cd /path/to/finalproject

# Build the project
make

# The executable will be created as: stock_analyzer
```

#### Manual Compilation (if Make is unavailable):

```bash
g++ -std=c++17 -Wall -Wextra -O3 -fopenmp -I./include \
    src/TechnicalIndicator.cpp src/Scheduler.cpp src/main.cpp \
    -o stock_analyzer -fopenmp
```

#### On macOS (if testing locally):

```bash
# If OpenMP is not available, install libomp first:
brew install libomp

# Then build normally:
make

# Or build without OpenMP:
make no-openmp
```

### Clean Build Artifacts

```bash
make clean
```

### Verification

After building, verify the executable was created:
```bash
ls -lh stock_analyzer
./stock_analyzer 5  # Quick test with 5 stocks
```

## Usage

### Basic Usage

```bash
# Run with default settings (10 stocks, benchmark mode)
./stock_analyzer

# Run with specific number of stocks
./stock_analyzer 50

# Run with scheduler mode (real-time analysis)
./stock_analyzer 10 scheduler

# Run without benchmark
./stock_analyzer 20 scheduler no-benchmark
```

### Makefile Targets

```bash
# Run with 10 stocks
make run

# Run with scheduler mode
make run-scheduler

# Benchmark with 50 stocks
make benchmark

# Large scale test (100 stocks)
make test-large

# Run unit tests
make test
```

## Performance Metrics

The system measures and reports:

- **Sequential execution time**: Baseline performance
- **Parallel execution time**: OpenMP-accelerated performance
- **Speedup**: Ratio of sequential to parallel time
- **Efficiency**: Speedup normalized by number of threads
- **Correctness validation**: Ensures parallel results match sequential results
- **Visual Performance Metrics**: ASCII-based charts and plots for:
  - Speedup visualization (actual vs theoretical)
  - Efficiency charts
  - Execution time comparisons
  - Comprehensive performance reports

### Expected Performance

- **Target speedup**: At least 30% improvement over sequential
- **Ideal performance**: Reduce computation time from minutes to seconds
- **Scalability**: Linear scaling with number of CPU cores (up to Amdahl's limit)

## Technical Details

### Technical Indicators

The system computes:

- **SMA (Simple Moving Average)**: 20-day and 50-day moving averages
- **RSI (Relative Strength Index)**: 14-period RSI
- **MACD (Moving Average Convergence Divergence)**: MACD line and signal line

### Signal Generation

Signals are generated based on:
- RSI levels (oversold < 30, overbought > 70)
- MACD crossover patterns
- Moving average crossovers
- Signal strength calculation

### Threading Architecture

1. **Scheduler Thread**: Triggers periodic analysis cycles
2. **Data Fetcher Thread**: Fetches real-time market data using socket connections
3. **Notification Dispatcher Thread**: Handles buy/sell signal alerts
4. **OpenMP Threads**: Parallel computation threads for indicator calculations

### Socket Programming

The system uses POSIX sockets for network communication:
- **TCP/IP Socket Connections**: Establishes connections to financial data APIs
- **HTTP Client**: Implements HTTP GET requests over sockets
- **Real-Time Data Fetching**: Retrieves stock market data via network sockets
- **Error Handling**: Graceful fallback if network is unavailable

## Project Structure

```
finalproject/
├── include/
│   ├── TechnicalIndicator.h    # Indicator computation class
│   ├── Scheduler.h             # Orchestration and scheduling
│   ├── ThreadSafeQueue.h       # Thread-safe queue implementation
│   ├── StockDataFetcher.h      # Socket-based data fetching
│   └── PerformanceVisualizer.h # Visual performance metrics
├── src/
│   ├── TechnicalIndicator.cpp  # Indicator implementation
│   ├── Scheduler.cpp           # Scheduler implementation
│   ├── StockDataFetcher.cpp    # Socket implementation
│   ├── PerformanceVisualizer.cpp # Visual metrics implementation
│   └── main.cpp                # Main integration and testing
├── tests/
│   └── test_technical_indicator.cpp # Unit tests
├── Makefile                    # Build configuration
└── README.md                   # This file
```

## Project Requirements Compliance

### Custom Classes ✓
- **TechnicalIndicator**: Encapsulates technical indicator calculations
- **Scheduler**: Manages concurrent task orchestration
- **ThreadSafeQueue**: Thread-safe queue for inter-thread communication
- **StockDataFetcher**: Socket-based network data fetching

### Special Topics Used ✓
- **OpenMP**: Data parallelism for parallel indicator computation across stocks
- **std::thread**: Task parallelism for scheduler, data fetcher, and notification dispatcher threads
- **Sockets**: TCP/IP socket programming for real-time stock data fetching from APIs

### Measurable Results ✓
- Performance metrics: Sequential vs parallel execution time
- Speedup calculation: Ratio of sequential to parallel time
- Efficiency: Speedup normalized by thread count
- Correctness validation: Parallel results match sequential results
- Scalability testing: Performance with varying numbers of stocks

## Team Responsibilities

### Hersh Gupta
- Technical indicator computation class (TechnicalIndicator)
- OpenMP parallelization for data parallelism
- Performance optimization of calculation engine
- Unit tests for indicator correctness (tests/test_technical_indicator.cpp)

### Shreyashi Dutta
- Scheduler and orchestration layer (Scheduler class)
- Thread-safe queue implementation (ThreadSafeQueue class)
- Background thread management (data fetcher, notification dispatcher)
- Synchronization and thread safety using mutexes and condition variables

### Tanmay Arvind
- System integration (main.cpp)
- Performance benchmarking and validation
- Visual performance metrics (PerformanceVisualizer class)
- Scalability testing with varying stock counts
- Documentation and final report

## Testing

The system includes:

- **Unit Tests**: Comprehensive unit tests for TechnicalIndicator class
  - Run tests: `make test`
  - Tests cover: SMA, RSI, MACD calculations, signal generation, parallel consistency, edge cases
- **Correctness validation**: Compares parallel vs sequential results
- **Performance benchmarking**: Measures speedup and efficiency with visual metrics
- **Scalability testing**: Tests with varying numbers of stocks
- **Stress testing**: Validates system under load

## Future Enhancements

- Real API integration for market data
- Additional technical indicators
- Machine learning-based signal generation
- Distributed computing support
- Real-time visualization dashboard

## License

This project is developed for ECE 6122 course requirements.

## Contact

For questions or issues, please contact the team members.

