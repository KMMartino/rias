#include "Plot.h"

Plot::Plot(const PlotStyle& style)
    : m_style(style) {
    m_pointsGlobal.reserve(style.historySize + 5);
    m_pointsLocal.reserve(style.historySize + 5);
}

void Plot::addValue(double val) {
    m_history.push_back(val);
    if (m_history.size() > m_style.historySize) {
        m_history.pop_front();
    }
}

void Plot::draw(cv::Mat& canvas, cv::Ptr<cv::freetype::FreeType2>& ft2) {
    cv::Rect safeRect = m_style.rect & cv::Rect(0, 0, canvas.cols, canvas.rows);
    if (safeRect.empty()) return;
    
    cv::Mat roi = canvas(safeRect);

    m_bgLayer.create(roi.size(), roi.type());
    m_bgLayer.setTo(cv::Scalar(20, 20, 20)); 
    cv::addWeighted(m_bgLayer, 0.6, roi, 0.4, 0, roi);

    int centerX = safeRect.width / 2;
    cv::line(roi, cv::Point(centerX, 0), cv::Point(centerX, safeRect.height), 
                cv::Scalar(255, 255, 255), 3, cv::LINE_AA);

    auto getY = [&](double val) {
        double clamped = std::max(m_style.minVal, std::min(val, m_style.maxVal));
        double ratio = (clamped - m_style.minVal) / (m_style.maxVal - m_style.minVal);
        return safeRect.height - (int)(ratio * safeRect.height);
    };

    for (double lineVal : m_style.gridLines) {
        int yLocal = getY(lineVal);
        cv::line(roi, cv::Point(0, yLocal), cv::Point(safeRect.width, yLocal), 
                    cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
    }

    if (!m_history.empty()) {
        m_pointsGlobal.clear();
        m_pointsLocal.clear();
        m_pointsLocal.push_back(cv::Point(0, safeRect.height));
        
        for (size_t i = 0; i < m_history.size(); ++i) {
            int x = (int)((double)i / (m_style.historySize - 1) * safeRect.width);
            int y = getY(m_history[i]);
            m_pointsLocal.push_back(cv::Point(x, y));
            m_pointsGlobal.push_back(cv::Point(safeRect.x + x, safeRect.y + y));
        }
        m_pointsLocal.push_back(cv::Point(safeRect.width, safeRect.height));

        m_polyLayer.create(roi.size(), roi.type());
        m_polyLayer.setTo(cv::Scalar(0,0,0));
        const cv::Point* ppt[1] = { m_pointsLocal.data() };
        int npt[] = { (int)m_pointsLocal.size() };
        cv::fillPoly(m_polyLayer, ppt, npt, 1, m_style.fillColor, cv::LINE_AA);
        cv::addWeighted(m_polyLayer, 0.5, roi, 1.0, 0, roi);
        cv::polylines(canvas, m_pointsGlobal, false, m_style.lineColor, 2, cv::LINE_AA);
    }

    cv::rectangle(canvas, safeRect, cv::Scalar(255, 255, 255), 3, cv::LINE_AA);

    int fontSize = (int)(safeRect.height * 0.12);
    if (fontSize < 14) fontSize = 14;
    if (fontSize > 24) fontSize = 24;

    if (ft2) {
        for (double lineVal : m_style.gridLines) {
            int yLocal = getY(lineVal);
            std::string label = std::format("{:.1f}", lineVal);
            cv::Point labelPos(safeRect.x + safeRect.width + 8, safeRect.y + yLocal + (fontSize / 3));
            ft2->putText(canvas, label, labelPos, fontSize, cv::Scalar(200, 200, 200), -1, cv::LINE_AA, true);
        }
    }

    if (ft2) {
        int titleSize = fontSize * 2;
        int baseline = 0;
        cv::Size textSize = ft2->getTextSize(m_style.title, titleSize, -1, &baseline);
        
        cv::Point titlePos;
        int titleY = safeRect.y - 8;

        if (m_style.alignTitleRight) {
            titlePos = cv::Point(safeRect.x + safeRect.width - textSize.width, titleY);
        } else {
            titlePos = cv::Point(safeRect.x, titleY);
        }
        ft2->putText(canvas, m_style.title, titlePos + cv::Point(2,2), titleSize, cv::Scalar(0,0,0), -1, cv::LINE_AA, true);
        ft2->putText(canvas, m_style.title, titlePos, titleSize, cv::Scalar(255, 255, 255), -1, cv::LINE_AA, true);
    }
}