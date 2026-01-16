#include "FrameProcessor.h"

FrameProcessor::FrameProcessor(int pixel_diff_threshold)
    : m_pixelDiffThreshold(pixel_diff_threshold) {
}

bool FrameProcessor::is_frame_unique(const cv::Mat& current, const cv::Mat& previous) {
    if (current.empty() || previous.empty()) {
        return false;
    }

    // Convert to grayscale before processing
    cv::cvtColor(current, m_grayCurrent, cv::COLOR_BGR2GRAY);
    cv::cvtColor(previous, m_grayPrevious, cv::COLOR_BGR2GRAY);

    
    cv::absdiff(m_grayCurrent, m_grayPrevious, m_diff);

    // Binary Threshold
    // Any pixel diff > threshold becomes 255 (white), else 0 (black)
    cv::threshold(m_diff, m_diff, (double)m_pixelDiffThreshold, 255, cv::THRESH_BINARY);
    int non_zero = cv::countNonZero(m_diff);

    return (non_zero > 0);
}