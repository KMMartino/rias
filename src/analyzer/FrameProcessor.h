#pragma once
#include <opencv2/opencv.hpp>

class FrameProcessor {
public:
    explicit FrameProcessor(int pixelDiffThreshold = 30);
    bool is_frame_unique(const cv::Mat& current, const cv::Mat& previous);
    const cv::Mat& getDiff();

private:
    int m_pixelDiffThreshold;

    cv::Mat m_diff;
};