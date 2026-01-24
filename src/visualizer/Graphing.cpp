#include "Graphing.h"
#include <algorithm>

Graphing::Graphing(int vidWidth, int vidHeight, int vidFPS, std::vector<FrameData>&& fullData)
    :m_style(vidWidth, vidHeight, vidFPS), m_ft2(cv::freetype::createFreeType2()), m_frameIdx(0), m_fullData(std::move(fullData))
{
    m_ft2->loadFontData("C:/Windows/Fonts/arial.ttf", 0);
    m_pointsGlobal.reserve(m_style.historySize + 2);
    m_pointsLocal.reserve(m_style.historySize + 2);
}

void Graphing::manageStats(){
    if (m_frameIdx < m_fullData.size()) {
        m_current = m_fullData[m_frameIdx];
    } else if (!m_fullData.empty()) {
        m_current = m_fullData.back();
    }

    m_historyWindow.push_back(m_current);
    if (m_historyWindow.size() > m_style.historySize) {
        m_historyWindow.pop_front();
    }
}

int Graphing::draw(cv::Mat& canvas){
    manageStats();

    cv::Rect graphRect(m_style.padding, canvas.rows - m_style.height - m_style.padding, m_style.width, m_style.height);
    cv::Mat roi = canvas(graphRect);

    m_bgLayer.create(roi.size(), roi.type());
    m_bgLayer.setTo(cv::Scalar(20, 20, 20));
    cv::addWeighted(m_bgLayer, 0.7, roi, 0.3, 0, roi);

    m_pointsGlobal.clear();
    m_pointsLocal.clear();

    auto getY = [&](double ms){
        double clamped = std::max(0.0, std::min(ms, m_style.maxMs));
        double ratio = clamped / m_style.maxMs;
        return m_style.height - (int)(ratio * m_style.height);
    };

    m_pointsGlobal.push_back(cv::Point(graphRect.x, graphRect.y + m_style.height));
    m_pointsLocal.push_back(cv::Point(0, m_style.height));

    int currentSize = (int)m_historyWindow.size();

    for (size_t i = 0; i < currentSize; ++i) {
        int xOffset = (int)((double)i / (m_style.historySize - 1) * m_style.width);
        
        int yLocal = getY(m_historyWindow[i].frametime);
        
        m_pointsLocal.push_back(cv::Point(xOffset, yLocal));
        m_pointsGlobal.push_back(cv::Point(graphRect.x + xOffset, graphRect.y + yLocal));
    }

    m_pointsGlobal.push_back(cv::Point(graphRect.x + m_style.width, graphRect.y + m_style.height));
    m_pointsLocal.push_back(cv::Point(m_style.width, m_style.height));

    m_polyLayer.create(roi.size(), roi.type());
    roi.copyTo(m_polyLayer);
    const cv::Point* ppt[1] = {m_pointsLocal.data()};
    int npt[] = {(int)m_pointsLocal.size()};
    cv::fillPoly(m_polyLayer, ppt, npt, 1, cv::Scalar(0, 200, 0), cv::LINE_AA);
    cv::addWeighted(m_polyLayer, 0.5, roi, 0.5, 0, roi);

    int y60 = getY(16.66);
    int y30 = getY(33.33);

    cv::line(canvas, cv::Point(graphRect.x, y60), cv::Point(graphRect.x + m_style.width, y60), cv::Scalar(100, 255, 100), 2);
    cv::line(canvas, cv::Point(graphRect.x, y30), cv::Point(graphRect.x + m_style.width, y30), cv::Scalar(100, 100, 255), 1);

    m_ft2->putText(canvas, "16.6ms", cv::Point(graphRect.x + 10, y60 - 5), m_style.fontSizeTitle, cv::Scalar(150, 255, 150), -1, cv::LINE_AA, true);
    m_ft2->putText(canvas, "33.3ms", cv::Point(graphRect.x + 10, y30 - 5), m_style.fontSizeLabel, cv::Scalar(150, 150, 255), -1, cv::LINE_AA, true);

    cv::polylines(canvas, m_pointsGlobal, false, cv::Scalar(100, 255, 100), 1, cv::LINE_AA);

    m_frameIdx++;
    return m_frameIdx;
}
