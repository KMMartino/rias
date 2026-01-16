#include "FrameProcessor.h"

FrameProcessor::FrameProcessor(int pixel_diff_threshold)
    : m_pixel_diff_threshold(pixel_diff_threshold) {
}

bool FrameProcessor::is_frame_unique(const cv::Mat& current, const cv::Mat& previous) {
    if (current.empty() || previous.empty()) {
        return false;
    }

    // Convert to grayscale before processing
    cv::cvtColor(current, m_gray_current, cv::COLOR_BGR2GRAY);
    cv::cvtColor(previous, m_gray_previous, cv::COLOR_BGR2GRAY);

    // Absolute Difference
    cv::absdiff(m_gray_current, m_gray_previous, m_diff);

    // Binary Threshold
    // Any pixel diff > threshold becomes 255 (white), else 0 (black)
    cv::threshold(m_diff, m_diff, (double)m_pixel_diff_threshold, 255, cv::THRESH_BINARY);

    // Count changed pixels
    int non_zero = cv::countNonZero(m_diff);

    return (non_zero > 0);
}