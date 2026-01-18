#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "FrameProcessor.h"

struct AnalysisResult{
    double timestampSec;
    double totalAverageFramerate;
    double currentFps;
    double frametime;
    bool uniqueFrame;
};

class Analyzer{
public:
    Analyzer(int threshold, bool reportFlag);
    
    bool analyze(const std::string& videoPath);
    void exportCsv(const std::string& outputPath) const;

private:
    FrameProcessor m_processor;
    std::vector<AnalysisResult> m_results;
    std::vector<AnalysisResult> m_resultsUnique;
    
    std::vector<uint8_t> m_fpsBuffer; 
    size_t m_bufferIdx;
    double m_recordedFps;
    unsigned int m_totalFrames;
    int m_bufferSize;
    bool m_reportFlag;
    unsigned int m_uniqueFrames;

    double calculateFrametime(size_t currentBufferIdx);
    void init(const cv::VideoCapture& capture);
    void printReport(long long& loopDurationm);
    double getLowFps(std::map<unsigned int, int> histogram, double percentile);
    void process(int& frameCounter, bool& unique);
};