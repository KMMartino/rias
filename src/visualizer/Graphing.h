#pragma once
#include "CsvLoader.h"
#include "Plot.h"


class Graphing{
public:
    Graphing(int vidWidth, int vidHeight, int vidFPS, std::vector<FrameData>&& fullData);

    int draw(cv::Mat& canvas);

private:
    void manageStats();
    cv::Ptr<cv::freetype::FreeType2> m_ft2;
    std::deque<FrameData> m_historyWindow;
    FrameData m_current;
    int m_frameIdx;
    std::vector<FrameData> m_fullData;
    std::unique_ptr<Plot> m_fpsPlot;
    std::unique_ptr<Plot> m_frametimePlot;
    int m_videoFPS = 60;
};