#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "FrameProcessor.h"
#include "ArgumentParser.h"

struct AnalysisResult{
    double timestampSec;
    double totalAverageFramerate;
    double currentFps;
    double frametime;
    bool uniqueFrame;
};

class Analyzer{
public:
    explicit Analyzer(const riasConfig& config);
    
    bool analyze();
    void exportCsv(const std::string& outputPath) const;

private:
    FrameProcessor m_processor;
    riasConfig m_config; 
    size_t m_bufferIdx;
    double m_recordedFps;
    unsigned int m_uniqueFrames;

    std::vector<uint8_t> m_fpsBuffer;
    unsigned int m_totalFrames;
    int m_bufferSize;

    std::vector<AnalysisResult> m_results;
    std::vector<AnalysisResult> m_resultsUnique;
    
    double calculateFrametime(size_t currentBufferIdx);
    void init(const cv::VideoCapture& capture);
    void printReport(long long& loopDurationm);
    double getLowFps(const std::map<unsigned int, int>& histogram, double percentile);
    void process(const int& frameCounter, const bool& unique);
    void diffView(int& consecutiveDupes, cv::Mat& lastUniqueDiffBgr, cv::Mat& currentFrame, bool unique);
};