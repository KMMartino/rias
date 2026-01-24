#pragma once
#include "v_ArgumentParser.h"
#include "FFmpegPipe.h"
#include "Graphing.h"
#include <opencv2/opencv.hpp>


class Renderer{
public:
    Renderer(const visualizerConfig& config);
    void run();

private:
    visualizerConfig m_config;
    cv::VideoCapture m_cap;
    int m_width, m_height, m_fps, m_totalFrames;
};