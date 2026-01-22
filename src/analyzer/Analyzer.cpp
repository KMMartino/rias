#include "Analyzer.h"
#include <print>
#include <numeric>
#include <fstream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <algorithm>
#include <utility>

Analyzer::Analyzer(const riasConfig& config)
    : m_processor(config.threshold)
    , m_config(config)
    , m_bufferIdx(0)
    , m_recordedFps(0.0)
    , m_uniqueFrames(0)
{
}

bool Analyzer::analyze(){
    cv::VideoCapture cap(m_config.inPath, cv::CAP_FFMPEG);
    if(!cap.isOpened()){
        std::println(stderr, "Error: Could not open video: {}", m_config.inPath);
        return false;
    }

    init(cap);

    cv::Mat frameBuffer1, frameBuffer2;

    cv::Mat* pCurrentFrame = &frameBuffer1;
    cv::Mat* pPrevFrame = &frameBuffer2;

    std::println("Starting analysis on {} frames @ {} fps...", m_totalFrames, m_recordedFps);

    //manually process first frame
    cap >> *pPrevFrame;
    if(pPrevFrame->empty()) return false;

    m_uniqueFrames = 1;
    m_fpsBuffer[0] = 1;
    m_bufferIdx = 1;
    
    cv::Mat lastUniqueDiffBgr = cv::Mat::zeros(pPrevFrame->size(), pPrevFrame->type());
    int consecutiveDupes = 0;

    //process rest of the frames
    int frameCounter = 1;
    auto loopTimeStart = std::chrono::high_resolution_clock::now();
    while(true){  
        cap >> *pCurrentFrame;
        if(pCurrentFrame->empty()) break;
        
        bool unique = 0;

        if(m_processor.is_frame_unique(*pCurrentFrame, *pPrevFrame)){
            unique = 1;
            m_uniqueFrames++;
        }
        m_fpsBuffer[m_bufferIdx] = unique;

        process(frameCounter, unique);

        if(m_config.tuningSet == true && frameCounter == m_bufferSize - 1){
            std::println("\nTuning limit reached. Stopping analysis.");
            break;
        }

        frameCounter ++;

        if(m_config.diffView){
            diffView(consecutiveDupes, lastUniqueDiffBgr, *pCurrentFrame, *pPrevFrame, unique);
            int key = cv::waitKey(m_config.delay);
        
            if (key == 27) {
                std::println("\nUser interrupted analysis via keyboard.");
                break;
            }
        }

        std::swap(pPrevFrame, pCurrentFrame);
    }
    auto loopTimeEnd = std::chrono::high_resolution_clock::now();
    auto loopDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loopTimeEnd - loopTimeStart).count();
    
    if(m_config.tuningSet == true){
        printTuningReport();
    }else{
        printReport(loopDuration);
    }
    
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
    if(m_config.report){
        for(size_t i = 0; i < m_resultsUnique.size(); ++i){
            const auto& res = m_resultsUnique[i];
            file << std::format("{:.3f},{:.1f},{:.1f},{:.2f}\n", res.timestampSec, res.totalAverageFramerate, res.currentFps, res.frametime);
        }
    }
    else{
        for(size_t i = 0; i < m_results.size(); ++i){
            const auto& res = m_results[i];
            if(res.uniqueFrame){
                file << std::format("{:.3f},{:.1f},{:.1f},{:.2f}\n", res.timestampSec, res.totalAverageFramerate, res.currentFps, res.frametime);
            }
        }
    }
    std::println("Exported CSV to: {}", outputPath);
}

void Analyzer::exportCsv_full(const std::string& outputPath) const {
    std::ofstream file(outputPath);
    if(!file.is_open()){
        std::println(stderr, "Failed to open output file: {}", outputPath);
        return;
    }

    file << "Time(s),fps(total),fps(current),Frametime(ms),Unique(bool)\n";
    double baseFrametime = 1000.0 / m_recordedFps;
    double frameDuration = baseFrametime;

    for(const auto& res: m_results){
        if(res.uniqueFrame){
            frameDuration = baseFrametime;
        }else{
            frameDuration += baseFrametime;
        }
        file << std::format("{:.3f},{:.1f},{:.1f},{:.2f},{}\n", res.timestampSec, res.totalAverageFramerate, res.currentFps, frameDuration, res.uniqueFrame);
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
    m_uniqueFrames = 0;

    m_results.clear();
    m_resultsUnique.clear();
    m_results.reserve(m_totalFrames);
}

void Analyzer::printReport(long long& loopDuration){
    double processingSpeed = 0.0;
    if(loopDuration > 0){
        processingSpeed = m_totalFrames * 1000 / loopDuration;
    }
    std::println("\nMain loop took: {} ms", loopDuration);
    std::println("Processing Speed: {:.1f} fps", processingSpeed);

    if(m_config.report){
        std::vector<double> frametimes;
        m_resultsUnique.reserve(m_results.size() / 2);
        std::map<unsigned int, int> frametimeHistogram;
        double stutterThreshold_ms = 2500 / m_recordedFps;
        unsigned int stutterCount = 0;
        for(size_t i=1; i < m_results.size(); ++i){
            const auto& res = m_results[i];
            if(res.uniqueFrame){
                unsigned int key = std::llround(res.frametime * 1000);
                frametimeHistogram[key]++;
                m_resultsUnique.push_back(res);
                if(res.frametime > stutterThreshold_ms) stutterCount++;
            }
        }
        std::println("----------\ndetailed report\n");
        std::println("1% low fps: {}fps", getLowFps(frametimeHistogram, 0.01));
        std::println("0.1% low fps: {}fps", getLowFps(frametimeHistogram, 0.001));
        std::println("stutters: -{}- major stutters were detedted (3 or more consecutive frames)", stutterCount);
        std::println("\nreport end\n----------");
    }
    
    std::println("Analysis Complete.");
}

void Analyzer::printTuningReport(){
    int diff = m_uniqueFrames - m_config.tuning;
    std::println("----------\ntuning report\n");
    std::println("Target FPS: {}", m_config.tuning);
    std::println("Detected FPS: {}", m_uniqueFrames);
    std::println("Threshold Used: {}\n", m_config.threshold);

    if(diff == 0){
        std::println("RESULT: PERFECT MATCH");
        std::println(">> This threshold is accurate for this footage.");
    }
    if(diff > 0){
        std::println("RESULT: ADJUSTMENT REQUIRED");
        std::println(">> Rias overcounted by {} at threshold {}. Try setting a higher threshold", diff, m_config.threshold);
    }
    if(diff < 0){
        std::println("RESULT: ADJUSTMENT REQUIRED");
        std::println(">> Rias undercounted by {} at threshold {}. Try setting a lower threshold", -diff, m_config.threshold);
    }
    std::println("\nreport end\n----------");
}

double Analyzer::getLowFps(const std::map<unsigned int, int>& histogram, double percentile){
    unsigned int targetCount = std::llround(m_uniqueFrames * percentile);
    if(targetCount == 0) return 0.0;

    unsigned int accumulatedTime_micsec = 0;
    unsigned int neededFrames = targetCount;
    for(auto it = histogram.rbegin(); it != histogram.rend(); ++it){
        unsigned int takeCount = std::min((unsigned int)it->second, neededFrames);
        accumulatedTime_micsec += (it->first * takeCount);
        neededFrames -= takeCount;
        if(neededFrames <= 0) break;
    }

    return 1000000.0 * targetCount / accumulatedTime_micsec;
}

void Analyzer::process(const int& frameCounter, const bool& unique){
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
    double totalAveFramerate = m_uniqueFrames / currentDuration;

    m_results.push_back({currentDuration, totalAveFramerate, currentFps, currentFrametime, unique});
    
    // increment rolling 1s buffer modulo framerate
    m_bufferIdx = (m_bufferIdx + 1) % m_bufferSize;

    if(frameCounter % (5 * m_bufferSize) == 0){
        std::print("\rProcessing: {:.1f}%", (frameCounter / (double)m_totalFrames) * 100.0);
    }
}

void Analyzer::diffView(int& consecutiveDupes, cv::Mat& lastUniqueDiffBgr, cv::Mat& currentFrame, cv::Mat& previousFrame, bool unique){
    if(unique){
        consecutiveDupes = 0;
        cv::absdiff(currentFrame, previousFrame, lastUniqueDiffBgr);
        lastUniqueDiffBgr *= 5;
        cv::imshow("Preview", lastUniqueDiffBgr);
    }
    else{
        consecutiveDupes++;

        cv::Mat coloredFrame = lastUniqueDiffBgr.clone();

        std::vector<cv::Mat> channels;
        cv::split(coloredFrame, channels);

        if (consecutiveDupes == 1) {
            channels[0] = cv::Scalar(0); 
        } 
        else {
            channels[0] = cv::Scalar(0);
            channels[1] = cv::Scalar(0);
        }

        cv::merge(channels, coloredFrame);
        cv::imshow("Preview", coloredFrame);
    }
}