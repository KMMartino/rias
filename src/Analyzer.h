#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "FrameProcessor.h"

// struct to hold analysis data per frame
struct AnalysisResult{
    double timestampSec;
    double toalAverageFramerate;
    double currentFps;
    double frametime;
    bool uniqueFrame;
};

class Analyzer{
public:
    Analyzer(int threshold);
    
    bool analyze(const std::string& videPath);
    void exportCsv(const std::string& outputPath) const;

private:
    FrameProcessor m_processor;
    std::vector<AnalysisResult> m_results;
    
    std::vector<uint8_t> m_fpsBuffer; 
    size_t m_bufferIdx;
    double m_recordedFps;
    double calculateFrametime(size_t currentBufferIdx);
};