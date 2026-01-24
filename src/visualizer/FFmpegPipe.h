#pragma once
#include <string>
#include <cstdio>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <print>
#include "v_ArgumentParser.h"

class FFmpegPipe {
public:
    FFmpegPipe(int width, int height, int fps, const visualizerConfig& config);
    ~FFmpegPipe();

    void write(const cv::Mat& frame);

private:
    FILE* pipe = nullptr;
};