#include "FrameProcessor.h"

FrameProcessor::FrameProcessor(int pixelDiffThreshold)
    : m_pixelDiffThreshold(pixelDiffThreshold) {
}

bool FrameProcessor::is_frame_unique(const cv::Mat& current, const cv::Mat& previous) {
    if (current.empty() || previous.empty()) {
        return false;
    }

    cv::absdiff(current, previous, m_diff);
    cv::Matx13f t(1.0f, 1.0f, 1.0f);
    cv::transform(m_diff, m_sumDiff, t);

    int count = cv::countNonZero(m_sumDiff > m_pixelDiffThreshold);

    return (count > 5);
}

const cv::Mat& FrameProcessor::getDiff(){
    return m_diff;
}