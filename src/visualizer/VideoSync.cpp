#include "VideoSync.h"
#include <print>
#include <limits>

int VideoSync::getOffset(const std::string& lowResPath, const std::string& highResPath){
    cv::VideoCapture capLow(lowResPath);
    cv::VideoCapture capHigh(highResPath);

    if (!capLow.isOpened() || !capHigh.isOpened()) {
        std::println(stderr, "Warning: Sync failed. Could not open videos.");
        return 0;
    }

    cv::Mat raw, processed;
    std::vector<cv::Mat> signature;
    capLow >> raw; 

    for (int i = 0; i < 3; ++i) {
        capLow >> raw;
        if (raw.empty()) return 0;
        preprocess(raw, processed);
        signature.push_back(processed);
    }

    std::vector<cv::Mat> window;
    double minDiff = std::numeric_limits<double>::max();
    int bestOffset = 0;

    for (int i = 0; i < 10; ++i) {
        capHigh >> raw;
        if (raw.empty()) break;

        preprocess(raw, processed);
        window.push_back(processed);

        if (window.size() < 3) continue;
        if (window.size() > 3) window.erase(window.begin());
        double diff = calculateDiff(signature, window);

        if (diff < minDiff) {
            minDiff = diff;
            bestOffset = i - 4;
        }
    }
    std::println("Sync locked. High-Res is ahead by {} frames.", bestOffset);
    return bestOffset;
}

void VideoSync::preprocess(const cv::Mat& src, cv::Mat& dst){
    cv::resize(src, dst, cv::Size(160, 90));
    cv::cvtColor(dst, dst, cv::COLOR_BGR2GRAY);
}

double VideoSync::calculateDiff(const std::vector<cv::Mat>& refFrames, const std::vector<cv::Mat>& targetFrames) {
    double totalDiff = 0.0;
    for (int i = 0; i < refFrames.size(); ++i) {
        cv::Mat diff;
        cv::absdiff(refFrames[i], targetFrames[i], diff);
        totalDiff += cv::sum(diff)[0];
    }
    return totalDiff;
}