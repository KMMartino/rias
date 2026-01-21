#include "FrameProcessor.h"

FrameProcessor::FrameProcessor(int pixel_diff_threshold)
    : m_pixelDiffThreshold(pixel_diff_threshold) {
}

bool FrameProcessor::is_frame_unique(const cv::Mat& current, const cv::Mat& previous) {
    if (current.empty() || previous.empty()) {
        return false;
    }

    static cv::Mat smallCur, smallPrev;
    static cv::Mat smallCurGreen, smallPrevGreen;

    cv::resize(current, smallCur, cv::Size(480, 270), 0, 0, cv::INTER_NEAREST);
    cv::resize(previous, smallPrev, cv::Size(480, 270), 0, 0, cv::INTER_NEAREST);

    cv::extractChannel(smallCur, smallCurGreen, 1);
    cv::extractChannel(smallPrev, smallPrevGreen, 1);

    
    cv::absdiff(smallCurGreen, smallPrevGreen, m_diff);
    cv::threshold(m_diff, m_diff, (double)m_pixelDiffThreshold, 255, cv::THRESH_BINARY);
    int non_zero = cv::countNonZero(m_diff);

    return (non_zero > 0);
}

const cv::Mat& FrameProcessor::getDiff(){
    return m_diff;
}