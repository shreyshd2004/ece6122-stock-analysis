#ifndef PERFORMANCE_VISUALIZER_H
#define PERFORMANCE_VISUALIZER_H

#include <vector>
#include <string>

class PerformanceVisualizer {
public:
    static void plotSpeedup(double speedup, int numThreads);
    static void plotEfficiency(double efficiency, int numThreads);
    static void plotExecutionTime(double sequentialTime, double parallelTime);
    static void plotScalability(const std::vector<int>& stockCounts, 
                                const std::vector<double>& speedups);
    static void generateReport(double sequentialTime, double parallelTime,
                              double speedup, double efficiency, int numThreads);
    static void generateAnalysis(double speedup, double efficiency, int numThreads, int numStocks);

private:
    static std::string createBar(double value, double maxValue, int width);
};

#endif
