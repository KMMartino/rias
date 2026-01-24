#pragma once
#include <deque>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include "CsvLoader.h"

struct GraphStyle {
    int width;
    int height;
    int padding;
    int fontSizeTitle;
    int fontSizeLabel;
    int lineThickness;
    double maxMs;
    int historySize;

    GraphStyle(int vidWidth, int vidHeight, int vidFPS) {
        width = (int)(vidWidth * 0.3);
        height = (int)(vidHeight * 0.28);
        padding = (int)(height / 6);
        fontSizeTitle = (int)(height / 12);
        fontSizeLabel = (int)(height / 15);
        lineThickness = std::max(1, (int)(height / 150));
        maxMs = 3000.0 / vidFPS;
        historySize = 3 * vidFPS;
    }
};

class Graphing{
public:
    Graphing(int vidWidth, int vidHeight, int vidFPS, std::vector<FrameData>&& fullData);

    void manageStats();
    int draw(cv::Mat& canvas);

private:
    GraphStyle m_style;
    cv::Ptr<cv::freetype::FreeType2> m_ft2;
    std::deque<FrameData> m_historyWindow;
    FrameData m_current;
    int m_frameIdx;
    std::vector<FrameData> m_fullData;
    cv::Mat m_bgLayer;
    cv::Mat m_polyLayer;
    std::vector<cv::Point> m_pointsGlobal;
    std::vector<cv::Point> m_pointsLocal;
};