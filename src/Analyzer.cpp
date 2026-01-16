#include "Analyzer.h"
#include <print>
#include <numeric>
#include <fstream>
#include <cmath>
#include <opencv2/opencv.hpp>

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

    m_recordedFps = cap.get(cv::CAP_PROP_FPS);
    if (m_recordedFps <= 0) m_recordedFps = 60;

    int totalFrames = (int)cap.get(cv::CAP_PROP_FRAME_COUNT);
    int bufferSize = (int)std::ceil(m_recordedFps);

    m_fpsBuffer.assign(bufferSize, 0); 
    m_bufferIdx = 0;
    m_results.clear();
    m_results.reserve(totalFrames);

    cv::Mat currentFrame, prevFrame;
    bool firstFrame = true;
    int frameCounter = 0;

    std::println("Starting analysis on {} frames @ {} fps...", totalFrames, m_recordedFps);

    while(true){  
        cap >> currentFrame;
        if(currentFrame.empty()) break;
        
        bool unique = 0;

        if(firstFrame){
            unique = 1;
            prevFrame = currentFrame.clone();
            firstFrame = false;
        }else{
            if(m_processor.is_frame_unique(currentFrame, prevFrame)){
                unique = 1;
            }
            prevFrame = currentFrame.clone();
        }

        m_fpsBuffer[m_bufferIdx] = unique;

        double currentFps = std::accumulate(m_fpsBuffer.begin(), m_fpsBuffer.end(), 0.0);
        if(frameCounter < bufferSize){
            currentFps *= (double)bufferSize / frameCounter;
        }

        double currentFrametime = 0.0;
        if(unique){
            currentFrametime = calculateFrametime(m_bufferIdx);
        }

        double timestamp = frameCounter / m_recordedFps;
        m_results.push_back({timestamp, currentFps, currentFrametime, unique});

        m_bufferIdx = (m_bufferIdx + 1) % bufferSize;
        frameCounter ++;

        if(frameCounter % 100 == 0){
            std::print("\rProcessing: {:.1f}%", (frameCounter / (double)totalFrames) * 100.0);
        }
    }

    std::println("\nAnalysis Complete.");
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

    file << "Time(s),FPS,Frametime(ms)\n";
    for(size_t i=1; i < m_results.size(); ++i){
        const auto& res = m_results[i];
        if(res.uniqueFrame){
            file << std::format("{:.3f},{:.1f},{:.2f}\n", res.timestampSec, res.fps, res.frametime);
        }
    }
    std::println("Exported CSV to: {}", outputPath);
}