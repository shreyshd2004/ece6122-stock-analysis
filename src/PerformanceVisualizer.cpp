#include "../include/PerformanceVisualizer.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

std::string PerformanceVisualizer::createBar(double value, double maxValue, int width) {
    int barLength = static_cast<int>((value / maxValue) * width);
    std::string bar(barLength, '#');
    return bar;
}

void PerformanceVisualizer::plotSpeedup(double speedup, int numThreads) {
    std::cout << "\n=== Speedup Visualization ===\n";
    std::cout << "Speedup: " << std::fixed << std::setprecision(2) << speedup << "x\n";
    std::cout << "Theoretical Max: " << numThreads << "x\n\n";
    
    double maxValue = std::max(speedup, static_cast<double>(numThreads));
    int barWidth = 50;
    
    std::cout << "Actual:   " << createBar(speedup, maxValue, barWidth) 
              << " " << std::setprecision(2) << speedup << "x\n";
    std::cout << "Theoretical: " << createBar(numThreads, maxValue, barWidth) 
              << " " << numThreads << "x\n";
    std::cout << "\n";
}

void PerformanceVisualizer::plotEfficiency(double efficiency, int /*numThreads*/) {
    std::cout << "\n=== Efficiency Visualization ===\n";
    std::cout << "Efficiency: " << std::fixed << std::setprecision(1) 
              << (efficiency * 100) << "%\n\n";
    
    int barWidth = 50;
    std::cout << "Efficiency: " << createBar(efficiency, 1.0, barWidth) 
              << " " << std::setprecision(1) << (efficiency * 100) << "%\n";
    std::cout << "            ";
    for (int i = 0; i <= 10; ++i) {
        std::cout << (i % 5 == 0 ? "|" : " ");
    }
    std::cout << "\n            0%    20%   40%   60%   80%   100%\n\n";
}

void PerformanceVisualizer::plotExecutionTime(double sequentialTime, double parallelTime) {
    std::cout << "\n=== Execution Time Comparison ===\n\n";
    
    double maxTime = std::max(sequentialTime, parallelTime);
    int barWidth = 50;
    
    std::cout << "Sequential: " << createBar(sequentialTime, maxTime, barWidth) 
              << " " << std::fixed << std::setprecision(6) << sequentialTime << "s\n";
    std::cout << "Parallel:   " << createBar(parallelTime, maxTime, barWidth) 
              << " " << std::fixed << std::setprecision(6) << parallelTime << "s\n";
    
    if (sequentialTime > 0 && parallelTime > 0) {
        double improvement = ((sequentialTime - parallelTime) / sequentialTime) * 100.0;
        std::cout << "\nImprovement: " << std::fixed << std::setprecision(1) 
                  << improvement << "% faster\n\n";
    }
}

void PerformanceVisualizer::plotScalability(const std::vector<int>& stockCounts,
                                           const std::vector<double>& speedups) {
    if (stockCounts.size() != speedups.size() || stockCounts.empty()) {
        return;
    }
    
    std::cout << "\n=== Scalability Analysis ===\n";
    std::cout << "Speedup vs Number of Stocks\n\n";
    
    double maxSpeedup = *std::max_element(speedups.begin(), speedups.end());
    
    std::cout << "Stocks | Speedup\n";
    std::cout << "-------|--------\n";
    
    for (size_t i = 0; i < stockCounts.size(); ++i) {
        std::cout << std::setw(6) << stockCounts[i] << " | ";
        int barLength = static_cast<int>((speedups[i] / maxSpeedup) * 40);
        std::cout << std::string(barLength, '#') << " " 
                  << std::fixed << std::setprecision(2) << speedups[i] << "x\n";
    }
    std::cout << "\n";
}

void PerformanceVisualizer::generateReport(double sequentialTime, double parallelTime,
                                          double speedup, double efficiency, int numThreads) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║         PERFORMANCE ANALYSIS REPORT                   ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Sequential Time: " << std::setw(35) << std::fixed 
              << std::setprecision(6) << sequentialTime << "s ║\n";
    std::cout << "║ Parallel Time:   " << std::setw(35) << std::fixed 
              << std::setprecision(6) << parallelTime << "s ║\n";
    std::cout << "║ Speedup:        " << std::setw(35) << std::fixed 
              << std::setprecision(2) << speedup << "x ║\n";
    std::cout << "║ Efficiency:     " << std::setw(34) << std::fixed 
              << std::setprecision(1) << (efficiency * 100) << "% ║\n";
    std::cout << "║ Threads Used:   " << std::setw(36) << numThreads << " ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
}

