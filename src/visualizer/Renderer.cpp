#include "Renderer.h"

Renderer::Renderer(const visualizerConfig& config)
    :m_config(config), m_cap(cv::VideoCapture(config.videoPath, cv::CAP_FFMPEG))
{
    if (!m_cap.isOpened()) {
        throw std::runtime_error("Failed to open video file");
    }
    m_width = (int)m_cap.get(cv::CAP_PROP_FRAME_WIDTH);
    m_height = (int)m_cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    m_fps = (int)m_cap.get(cv::CAP_PROP_FPS);
    m_totalFrames = (int)m_cap.get(cv::CAP_PROP_FRAME_COUNT);
    if (m_fps <= 0) m_fps = 60;
    std::println("Input set: {}x{} at {}fps", m_width, m_height, m_fps);
}

void Renderer::run(){
    Graphing grapher = Graphing(m_width, m_height, m_fps, CsvLoader::load(m_config.csvPath));
    FFmpegPipe encoder = FFmpegPipe(m_width, m_height, m_fps, m_config.outPath);
    cv::Mat frame;
    std::println("Rendering {} frames...", m_totalFrames);
    while (true) {
        m_cap >> frame;
        if (frame.empty()) break;
        int index = grapher.draw(frame);
        encoder.write(frame);
        if (index % 60 == 0) {
            std::print("\rEncoding: {:.1f}%", (double)index / m_totalFrames * 100.0);
        }
    }
    std::println("\nDone! Saved to {}", m_config.outPath);
}