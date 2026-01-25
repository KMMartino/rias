#pragma once
#include <deque>
#include <string>
#include <vector>
#include <algorithm>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>

struct PlotStyle {
    cv::Rect rect;
    
    double minVal;
    double maxVal;
    
    cv::Scalar lineColor;
    cv::Scalar fillColor;
    std::string title;
    
    std::vector<double> gridLines;
    int historySize;
    bool alignTitleRight = false;
};

class Plot {
public:
    Plot(const PlotStyle& style);

    void addValue(double val);
    void draw(cv::Mat& canvas, cv::Ptr<cv::freetype::FreeType2>& ft2);

private:
    PlotStyle m_style;
    std::deque<double> m_history;
    
    cv::Mat m_bgLayer;
    cv::Mat m_polyLayer;
    std::vector<cv::Point> m_pointsGlobal;
    std::vector<cv::Point> m_pointsLocal;
};