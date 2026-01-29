#pragma once
#include <string>
#include <cstdio>
#include <opencv2/opencv.hpp>
#include "argParser.h"

class FFmpegPipe {
public:
    FFmpegPipe(int width, int height, int fps, const VisualizerConfig& config);
    ~FFmpegPipe();

    void write(const cv::Mat& frame);

private:
    FILE* pipe = nullptr;
};