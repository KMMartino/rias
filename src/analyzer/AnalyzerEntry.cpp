#include "Analyzer.h"

int runAnalysis(const AnalyzerConfig& config) {
    Analyzer analyzer(config);
    if (analyzer.analyze()) {
        analyzer.exportCsv_full(config.outPath);
    }
    return 0;
}