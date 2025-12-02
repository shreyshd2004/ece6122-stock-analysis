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
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║           SPEEDUP VISUALIZATION                       ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Actual Speedup:     " << std::setw(33) << std::fixed 
              << std::setprecision(2) << speedup << "x ║\n";
    std::cout << "║ Theoretical Max:    " << std::setw(33) << numThreads << "x ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    
    double maxValue = std::max(speedup, static_cast<double>(numThreads));
    int barWidth = 60;
    
    std::cout << "  Actual:     ";
    int actualBar = std::max(1, static_cast<int>((speedup / maxValue) * barWidth));
    std::cout << std::string(actualBar, '#') << std::string(barWidth - actualBar, ' ') 
              << " " << std::setprecision(2) << speedup << "x\n";
    
    std::cout << "  Theoretical: ";
    std::cout << std::string(barWidth, '#') << " " << numThreads << "x\n";
    
    double percentage = (speedup / numThreads) * 100.0;
    std::cout << "\n  Achievement: " << std::fixed << std::setprecision(1) 
              << percentage << "% of theoretical maximum\n";
    
    if (speedup < 1.0) {
        std::cout << "  Note: Overhead dominates for small datasets. Try with 100+ stocks.\n";
    }
    std::cout << "\n";
}

void PerformanceVisualizer::plotEfficiency(double efficiency, int numThreads) {
    (void)numThreads;
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║          EFFICIENCY VISUALIZATION                      ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Efficiency: " << std::setw(42) << std::fixed 
              << std::setprecision(1) << (efficiency * 100) << "% ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    
    int barWidth = 60;
    int efficiencyBar = static_cast<int>(efficiency * barWidth);
    efficiencyBar = std::max(1, efficiencyBar);
    
    std::cout << "  [";
    std::cout << std::string(efficiencyBar, '=');
    std::cout << std::string(barWidth - efficiencyBar, ' ');
    std::cout << "] " << std::fixed << std::setprecision(1) << (efficiency * 100) << "%\n";
    
    std::cout << "  0%";
    std::cout << std::string((barWidth / 2) - 3, ' ') << "50%";
    std::cout << std::string((barWidth / 2) - 3, ' ') << "100%\n\n";
}

void PerformanceVisualizer::plotExecutionTime(double sequentialTime, double parallelTime) {
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║        EXECUTION TIME COMPARISON                      ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    std::cout << "║ Sequential: " << std::setw(42) << std::fixed 
              << std::setprecision(6) << sequentialTime << "s ║\n";
    std::cout << "║ Parallel:   " << std::setw(42) << std::fixed 
              << std::setprecision(6) << parallelTime << "s ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n\n";
    
    double maxTime = std::max(sequentialTime, parallelTime);
    int barWidth = 60;
    
    std::cout << "  Sequential: ";
    int seqBar = static_cast<int>((sequentialTime / maxTime) * barWidth);
    seqBar = std::max(1, seqBar);
    std::cout << std::string(seqBar, '|') << std::string(barWidth - seqBar, ' ') 
              << " " << std::fixed << std::setprecision(6) << sequentialTime << "s\n";
    
    std::cout << "  Parallel:   ";
    int parBar = static_cast<int>((parallelTime / maxTime) * barWidth);
    parBar = std::max(1, parBar);
    std::cout << std::string(parBar, '|') << std::string(barWidth - parBar, ' ') 
              << " " << std::fixed << std::setprecision(6) << parallelTime << "s\n";
    
    if (sequentialTime > 0 && parallelTime > 0) {
        double improvement = ((sequentialTime - parallelTime) / sequentialTime) * 100.0;
        std::cout << "\n  ";
        if (improvement > 0) {
            std::cout << "Parallel is " << std::fixed << std::setprecision(1) 
                      << improvement << "% faster\n";
        } else {
            std::cout << "Sequential is " << std::fixed << std::setprecision(1) 
                      << std::abs(improvement) << "% faster (overhead for small datasets)\n";
            std::cout << "  Tip: Use 100+ stocks to see parallel speedup\n";
        }
    }
    std::cout << "\n";
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

void PerformanceVisualizer::generateAnalysis(double speedup, double efficiency, 
                                            int numThreads, int numStocks) {
    (void)efficiency;
    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║           PERFORMANCE ANALYSIS                         ║\n";
    std::cout << "╠════════════════════════════════════════════════════════╣\n";
    
    if (speedup < 1.0) {
        std::cout << "║ Status:        Overhead dominates (small dataset)      ║\n";
        std::cout << "║ Recommendation: Test with 100+ stocks for speedup    ║\n";
    } else if (speedup < numThreads * 0.5) {
        std::cout << "║ Status:        Moderate speedup achieved              ║\n";
        std::cout << "║ Recommendation: Good parallelization, can optimize   ║\n";
    } else {
        std::cout << "║ Status:        Excellent speedup achieved            ║\n";
        std::cout << "║ Recommendation: Optimal parallel performance         ║\n";
    }
    
    std::cout << "║ Dataset Size:  " << std::setw(38) << numStocks << " stocks ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
}
