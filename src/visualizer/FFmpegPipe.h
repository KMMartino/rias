#pragma once
#include <string>
#include <cstdio>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <print>

class FFmpegPipe {
public:
    FFmpegPipe(int width, int height, int fps, const std::string& outPath);
    ~FFmpegPipe();

    void write(const cv::Mat& frame);

private:
    FILE* pipe = nullptr;
};