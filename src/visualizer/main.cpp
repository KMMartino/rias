#include <print>
#include <deque>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include "ArgumentParser.h"
#include "CsvLoader.h"
#include "FFmpegPipe.h"

const int GRAPH_HEIGHT = 300;
const int GRAPH_WIDTH = 600;
const int PADDING = 50;
const double MAX_MS = 50.0;
const int HISTORY_SIZE = 120;

void drawGraph(cv::Mat& canvas, const std::deque<FrameData>& history, cv::Ptr<cv::freetype::FreeType2>& ft2);

int main(int argc, char** argv){
    if (argc < 3) {
        std::println("Usage: rias-visualizer.exe <video_path> <csv_path>");
        return 1;
    }
    std::string videoPath = argv[1];
    std::string csvPath = argv[2];
    std::string outPath = "rendered_output.mp4";

    std::println("Loading Data...");
    auto fullData = CsvLoader::load(csvPath);

    cv::VideoCapture cap(videoPath, cv::CAP_FFMPEG);
    if (!cap.isOpened()) {
        std::println(stderr, "Failed to open video: {}", videoPath);
        return 1;
    }

    cv::Ptr<cv::freetype::FreeType2> ft2 = cv::freetype::createFreeType2();
    try{
        ft2->loadFontData("C:/Windows/Fonts/arial.ttf", 0);
    } catch(const std::exception& e){
        std::println(stderr, "Failed to load font! Error: {}", e.what());
        return 1;
    }

    int width = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int fps = (int)cap.get(cv::CAP_PROP_FPS);
    if (fps <= 0) fps = 60;

    FFmpegPipe encoder(width, height, fps, outPath);

    cv::Mat frame;
    std::deque<FrameData> historyWindow;
    int frameIdx = 0;
    int totalFrames = (int)cap.get(cv::CAP_PROP_FRAME_COUNT);
    std::println("Rendering {} frames...", totalFrames);

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        FrameData currentStats;
        if (frameIdx < fullData.size()) {
            currentStats = fullData[frameIdx];
        } else if (!fullData.empty()) {
            currentStats = fullData.back();
        }

        historyWindow.push_back(currentStats);
        if (historyWindow.size() > HISTORY_SIZE) {
            historyWindow.pop_front();
        }

        drawGraph(frame, historyWindow, ft2);

        encoder.write(frame);

        if (frameIdx % 60 == 0) {
            std::print("\rEncoding: {:.1f}%", (double)frameIdx / totalFrames * 100.0);
        }
        frameIdx++;
    }
    std::println("\nDone! Saved to {}", outPath);
}


void drawGraph(cv::Mat& canvas, const std::deque<FrameData>& history, cv::Ptr<cv::freetype::FreeType2>& ft2){
    cv::Rect graphRect(PADDING, canvas.rows - GRAPH_HEIGHT - PADDING, GRAPH_WIDTH, GRAPH_HEIGHT);
    cv::Mat overlay;
    canvas.copyTo(overlay);
    cv::rectangle(overlay, graphRect, cv::Scalar(20, 20, 20), cv::FILLED);
    cv::addWeighted(overlay, 0.7, canvas, 0.3, 0, canvas);

    auto getY = [&](double ms){
        double clamped = std::max(0.0, std::min(ms, MAX_MS));
        double ratio = clamped / MAX_MS;
        return graphRect.y + GRAPH_HEIGHT - (int)(ratio * GRAPH_HEIGHT);
    };

    int y60 = getY(16.66);
    int y30 = getY(33.33);

    cv::line(canvas, cv::Point(graphRect.x, y60), cv::Point(graphRect.x + GRAPH_WIDTH, y60), cv::Scalar(100, 255, 100), 2);
    cv::line(canvas, cv::Point(graphRect.x, y30), cv::Point(graphRect.x + GRAPH_WIDTH, y30), cv::Scalar(100, 100, 255), 1);

    ft2->putText(canvas, "16.6ms", cv::Point(graphRect.x + 10, y60 - 5), 20, cv::Scalar(150, 255, 150), -1, cv::LINE_AA, true);
    ft2->putText(canvas, "33.3ms", cv::Point(graphRect.x + 10, y30 - 5), 15, cv::Scalar(150, 150, 255), -1, cv::LINE_AA, true);

    std::vector<cv::Point> points;
    points.push_back(cv::Point(graphRect.x, graphRect.y + GRAPH_HEIGHT));

    for (size_t i = 0; i < history.size(); ++i) {
        int x = graphRect.x + (int)((double)i / history.size() * GRAPH_WIDTH);
        int y = getY(history[i].frametime);
        points.push_back(cv::Point(x, y));
    }

    points.push_back(cv::Point(graphRect.x + GRAPH_WIDTH, graphRect.y + GRAPH_HEIGHT));


    const cv::Point* ppt[1] = {points.data()};
    int npt[] = {(int)points.size()};
    cv::Mat polyLayer = canvas.clone();
    cv::fillPoly(polyLayer, ppt, npt, 1, cv::Scalar(0, 200, 0), cv::LINE_AA);
    cv::addWeighted(polyLayer, 0.5, canvas, 0.5, 0, canvas);

    cv::polylines(canvas, points, false, cv::Scalar(100, 255, 100), 1, cv::LINE_AA);
}