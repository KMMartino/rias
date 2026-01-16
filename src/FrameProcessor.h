#pragma once
#include <opencv2/opencv.hpp>

class FrameProcessor {
public:
    // construct with pixel diff(0-255) to identify a pixel is different(usually 20-40)
    explicit FrameProcessor(int pixelDiffThreshold = 30);

    // compares 2 frames based on pixel diff and teturns true if different
    bool is_frame_unique(const cv::Mat& current, const cv::Mat& previous);

private:
    int m_pixelDiffThreshold;

    // Pre-allocated buffers to avoid memory churn per frame
    cv::Mat m_grayCurrent;
    cv::Mat m_grayPrevious;
    cv::Mat m_diff;
};