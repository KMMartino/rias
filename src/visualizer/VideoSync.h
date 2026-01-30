#pragma once
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

class VideoSync {
public:
    static int getOffset(const std::string& lowResPath, const std::string& highResPath);

private:
    static void preprocess(const cv::Mat& src, cv::Mat& dst);
    static double calculateDiff(const std::vector<cv::Mat>& refFrames, const std::vector<cv::Mat>& targetFrames);
};