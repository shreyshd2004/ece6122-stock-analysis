#ifndef PERFORMANCE_VISUALIZER_H
#define PERFORMANCE_VISUALIZER_H

#include <vector>
#include <string>

/**
 * PerformanceVisualizer Class
 * 
 * Generates ASCII-based visual performance metrics including
 * speedup plots, efficiency charts, and execution time comparisons.
 * 
 * Author: Tanmay Arvind
 */
class PerformanceVisualizer {
public:
    // Generate speedup bar chart
    static void plotSpeedup(double speedup, int numThreads);
    
    // Generate efficiency chart
    static void plotEfficiency(double efficiency, int numThreads);
    
    // Generate execution time comparison
    static void plotExecutionTime(double sequentialTime, double parallelTime);
    
    // Generate scalability plot (speedup vs number of stocks)
    static void plotScalability(const std::vector<int>& stockCounts, 
                                const std::vector<double>& speedups);
    
    // Generate comprehensive performance report
    static void generateReport(double sequentialTime, double parallelTime,
                              double speedup, double efficiency, int numThreads);
    
    // Generate performance analysis with recommendations
    static void generateAnalysis(double speedup, double efficiency, int numThreads, int numStocks);
    
private:
    static std::string createBar(double value, double maxValue, int width);
};

#endif // PERFORMANCE_VISUALIZER_H

