# Makefile for Parallel Stock Market Analysis System
# ECE 6122 Final Project

# Detect OS and set appropriate OpenMP flags
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS - requires libomp installed via: brew install libomp
    CXX = clang++
    OPENMP_FLAGS = -Xpreprocessor -fopenmp -lomp
    OPENMP_LDFLAGS = -lomp
else
    # Linux
    CXX = g++
    OPENMP_FLAGS = -fopenmp
    OPENMP_LDFLAGS = -fopenmp
endif

CXXFLAGS = -std=c++17 -Wall -Wextra -O3 $(OPENMP_FLAGS)
LDFLAGS = $(OPENMP_LDFLAGS)
INCLUDES = -I./include
SOURCES = src/TechnicalIndicator.cpp src/Scheduler.cpp src/main.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = stock_analyzer
TEST_TARGET = test_analyzer

# Default target
all: $(TARGET)

# Main executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET)
	@echo "Clean complete"

# Run the program
run: $(TARGET)
	./$(TARGET) 10

# Run with scheduler
run-scheduler: $(TARGET)
	./$(TARGET) 10 scheduler

# Benchmark mode
benchmark: $(TARGET)
	./$(TARGET) 50

# Large scale test
test-large: $(TARGET)
	./$(TARGET) 100

# Build without OpenMP (fallback)
no-openmp:
	$(CXX) -std=c++17 -Wall -Wextra -O3 -I./include \
		src/TechnicalIndicator.cpp src/Scheduler.cpp src/main.cpp \
		-o $(TARGET)
	@echo "Build complete (without OpenMP): $(TARGET)"

# Help
help:
	@echo "Available targets:"
	@echo "  all          - Build the main executable with OpenMP (default)"
	@echo "  no-openmp   - Build without OpenMP support"
	@echo "  clean       - Remove build artifacts"
	@echo "  run         - Run with 10 stocks"
	@echo "  run-scheduler - Run with scheduler mode"
	@echo "  benchmark   - Run benchmark with 50 stocks"
	@echo "  test-large  - Test with 100 stocks"
	@echo ""
	@echo "Usage: ./$(TARGET) [num_stocks] [scheduler] [no-benchmark]"
	@echo ""
	@echo "Note: On macOS, if OpenMP build fails, install libomp: brew install libomp"

.PHONY: all clean run run-scheduler benchmark test-large no-openmp help

