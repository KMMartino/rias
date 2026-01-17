#include "Analyzer.h"
#include <print>
#include <numeric>
#include <fstream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <chrono>

Analyzer::Analyzer(int threshold)
    :m_processor(threshold), m_bufferIdx(0), m_recordedFps(0.0)
{
}

bool Analyzer::analyze(const std::string& videoPath){
    cv::VideoCapture cap(videoPath, cv::CAP_FFMPEG);
    if(!cap.isOpened()){
        std::println(stderr, "Error: Could not open video: {}", videoPath);
        return false;
    }

    init(cap);

    cv::Mat currentFrame, prevFrame;
    bool firstFrame = true;
    int frameCounter = 0;
    int uniqueFrameCount = 0;

    std::println("Starting analysis on {} frames @ {} fps...", m_totalFrames, m_recordedFps);

    auto loopTimeStart = std::chrono::high_resolution_clock::now();
    while(true){  
        cap >> currentFrame;
        if(currentFrame.empty()) break;
        
        bool unique = 0;

        if(firstFrame){
            unique = 1;
            uniqueFrameCount++;
            prevFrame = currentFrame.clone();
            firstFrame = false;
        }else{
            if(m_processor.is_frame_unique(currentFrame, prevFrame)){
                unique = 1;
                uniqueFrameCount++;
            }
            prevFrame = currentFrame.clone();
        }
        m_fpsBuffer[m_bufferIdx] = unique;

        process(frameCounter, unique, uniqueFrameCount);
    }
    auto loopTimeEnd = std::chrono::high_resolution_clock::now();
    
    auto loopDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loopTimeEnd - loopTimeStart).count();
    printReport(loopDuration);
    
    return true;
}


double Analyzer::calculateFrametime(size_t currentIdx){
    int bufferSize = (int)m_fpsBuffer.size();
    int zeroCount = 0;

    int scanIdx = (int)currentIdx - 1;
    if(scanIdx < 0) scanIdx = bufferSize - 1;

    for(int i = 0; i < bufferSize; ++i){
        if(m_fpsBuffer[scanIdx] == 1){
            break;
        }
        zeroCount++;

        scanIdx--;
        if(scanIdx < 0) scanIdx = bufferSize - 1;
    }
    double frameDuration = 1000.0 / m_recordedFps;
    return (zeroCount + 1) * frameDuration;
}


void Analyzer::exportCsv(const std::string& outputPath) const {
    std::ofstream file(outputPath);
    if(!file.is_open()){
        std::println(stderr, "Failed to open output file: {}", outputPath);
        return;
    }

    file << "Time(s),fps(total),fps(current),Frametime(ms)\n";
    for(size_t i=1; i < m_results.size(); ++i){
        const auto& res = m_results[i];
        if(res.uniqueFrame){
            file << std::format("{:.3f},{:.1f},{:.1f},{:.2f}\n", res.timestampSec, res.toalAverageFramerate, res.currentFps, res.frametime);
        }
    }
    std::println("Exported CSV to: {}", outputPath);
}

void Analyzer::init(const cv::VideoCapture& capture){
    m_recordedFps = capture.get(cv::CAP_PROP_FPS);
    if (m_recordedFps <= 0) m_recordedFps = 60;

    m_totalFrames = (int)capture.get(cv::CAP_PROP_FRAME_COUNT);
    m_bufferSize = (int)std::ceil(m_recordedFps);
    m_fpsBuffer.assign(m_bufferSize, 0); 
    m_bufferIdx = 0;
    m_results.clear();
    m_results.reserve(m_totalFrames);
}

void Analyzer::printReport(long long& loopDuration){
    double processingSpeed = 0.0;
    if(loopDuration > 0){
        processingSpeed = m_totalFrames * 1000 / loopDuration;
    }
    std::println("\nMain loop took: {} ms", loopDuration);
    std::println("Processing Speed: {:.1f} fps", processingSpeed);
    std::println("\nAnalysis Complete.");
}

void Analyzer::process(int& frameCounter, bool& unique, int& uniqueFrameCount){
    //current fps
    double currentFps = std::accumulate(m_fpsBuffer.begin(), m_fpsBuffer.end(), 0.0);
    if(frameCounter < m_bufferSize){
        currentFps *= (double)m_bufferSize / (frameCounter + 1);
    }
    //current frame frametime
    double currentFrametime = 0.0;
    if(unique){
        currentFrametime = calculateFrametime(m_bufferIdx);
    }
    //timestamp
    double currentDuration = (frameCounter + 1) / m_recordedFps;
    //average fps
    double totalAveFramerate = uniqueFrameCount / currentDuration;

    m_results.push_back({currentDuration, totalAveFramerate, currentFps, currentFrametime, unique});
    
    // increment rolling 1s buffer modulo framerate
    m_bufferIdx = (m_bufferIdx + 1) % m_bufferSize;
    frameCounter ++;

    if(frameCounter % 5 * m_bufferSize == 0){
        std::print("\rProcessing: {:.1f}%", (frameCounter / (double)m_totalFrames) * 100.0);
    }
}