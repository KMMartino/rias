#include "Graphing.h"
#include <algorithm>
#include <format>

Graphing::Graphing(int vidWidth, int vidHeight, int vidFPS, std::vector<FrameData>&& fullData)
    :m_ft2(cv::freetype::createFreeType2()), m_frameIdx(0), m_fullData(std::move(fullData)), m_videoFPS(vidFPS)
{
    try {
        m_ft2->loadFontData("C:/Windows/Fonts/arialbd.ttf", 0);
    } catch (...) {
        try { m_ft2->loadFontData("C:/Windows/Fonts/arial.ttf", 0); } catch(...) {}
    }


    int margin = (int)(vidWidth * 0.02);
    int contentWidth = vidWidth - (2.5 * margin);

    PlotStyle fpsStyle;
    int fpsHeight = (int)(vidHeight * 0.18);
    fpsStyle.rect = cv::Rect(margin, vidHeight - margin - fpsHeight, contentWidth, fpsHeight);
    
    fpsStyle.minVal = 0.0;
    fpsStyle.maxVal = vidFPS;
    double gridLineRef = (double)vidFPS * 0.25;
    fpsStyle.gridLines = { gridLineRef, gridLineRef * 2, gridLineRef * 3, gridLineRef * 4 };
    fpsStyle.title = "FRAME-RATE (FPS)";
    fpsStyle.alignTitleRight = true;
    fpsStyle.lineColor = cv::Scalar(100, 255, 100);
    fpsStyle.lineWidth = 3;
    fpsStyle.historySize = vidFPS * m_fpsWindowSecs;

    m_fpsPlot = std::make_unique<Plot>(fpsStyle);

    PlotStyle ftStyle;
    int ftHeight = (int)(vidHeight * 0.15);
    int ftWidth  = (int)(vidWidth * 0.25);
    
    ftStyle.rect = cv::Rect(margin, fpsStyle.rect.y - margin - ftHeight, ftWidth, ftHeight);
    
    double frametimeBase = 1000.0 / vidFPS;
    ftStyle.minVal = 0.0;
    ftStyle.maxVal = frametimeBase * 4;
    ftStyle.gridLines = { frametimeBase, frametimeBase * 2, frametimeBase * 3, frametimeBase * 4 };
    ftStyle.title = "FRAME-TIME (MS)";
    ftStyle.alignTitleRight = false;
    ftStyle.lineColor = cv::Scalar(100, 100, 255);
    ftStyle.lineWidth = 2;
    ftStyle.historySize = vidFPS * m_ftWindowSecs;

    m_frametimePlot = std::make_unique<Plot>(ftStyle);

    auto getClampedData = [&](int idx) -> const FrameData& {
        if (idx < 0) return m_fullData.front();
        if (static_cast<size_t>(idx) >= m_fullData.size()) return m_fullData.back();
        return m_fullData[idx];
    };

    int fpsOffset = (int)(vidFPS * m_fpsWindowSecs / 2);
    for (int i = 0; i < fpsStyle.historySize - 1; i++) {
        int frameToLoad = i - (fpsStyle.historySize - 1) + fpsOffset;
        m_fpsPlot->addValue(getClampedData(frameToLoad).fpsCurrent);
    }

    int ftOffset = (int)(m_videoFPS * m_ftWindowSecs / 2);
    for (int i = 0; i < ftStyle.historySize - 1; i++) {
        int frameToLoad = i - (ftStyle.historySize - 1) + ftOffset;
        m_frametimePlot->addValue(getClampedData(frameToLoad).frametime);
    }
}

void Graphing::manageStats(){
    if (m_fullData.empty()) return;
    if (m_frameIdx < m_fullData.size()) {
        m_current = m_fullData[m_frameIdx];
    } else {
        m_current = m_fullData.back();
    }
}

int Graphing::draw(cv::Mat& canvas) {
    manageStats();

    auto getDataAt = [&](int idx) -> FrameData {
    if (idx < 0) return m_fullData.front();
    if (static_cast<size_t>(idx) >= m_fullData.size()) return m_fullData.back();
    return m_fullData[idx];
    };

    if (m_frametimePlot) {
        FrameData future = getDataAt(m_frameIdx + int(m_videoFPS * m_ftWindowSecs / 2));
        m_frametimePlot->addValue(future.frametime);
    }

    if (m_fpsPlot) {
        FrameData future = getDataAt(m_frameIdx + int(m_videoFPS * m_fpsWindowSecs / 2));
        m_fpsPlot->addValue(future.fpsCurrent);
    }

    if (m_fpsPlot)       m_fpsPlot->draw(canvas, m_ft2);
    if (m_frametimePlot) m_frametimePlot->draw(canvas, m_ft2);

    if (m_ft2) {
        std::string bigFps = std::format("{:.0f}", m_current.fpsCurrent);
        
        int bigSize = (int)(canvas.rows * 0.08); 
        
        int baseline = 0;
        cv::Size textSize = m_ft2->getTextSize(bigFps, bigSize, -1, &baseline);
        
        int margin = (int)(canvas.cols * 0.015);
        cv::Point pos(canvas.cols - margin - textSize.width, margin + textSize.height);
        
        m_ft2->putText(canvas, bigFps, pos + cv::Point(3,3), bigSize, cv::Scalar(0,0,0), -1, cv::LINE_AA, true);
        m_ft2->putText(canvas, bigFps, pos, bigSize, cv::Scalar(255, 255, 255), -1, cv::LINE_AA, true);
    }

    m_frameIdx++;
    return m_frameIdx;
}
