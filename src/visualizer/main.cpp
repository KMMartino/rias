#include <print>
#include <deque>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include "CsvLoader.h"
#include "FFmpegPipe.h"
#include "Graphing.h"
#include <chrono>


int main(int argc, char** argv){
    auto programTimeStart = std::chrono::high_resolution_clock::now();
    if (argc < 3) {
        std::println("Usage: rias-visualizer.exe <video_path> <csv_path>");
        return 1;
    }
    std::string videoPath = argv[1];
    std::string csvPath = argv[2];
    std::string outPath = "rendered_output.mp4";

    cv::VideoCapture cap(videoPath, cv::CAP_FFMPEG);
    if (!cap.isOpened()) {
        std::println(stderr, "Failed to open video: {}", videoPath);
        return 1;
    }
    int width = (int)cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = (int)cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int fps = (int)cap.get(cv::CAP_PROP_FPS);
    if (fps <= 0) fps = 60;
    std::println("Input set: {}x{} at {}fps", width, height, fps);

    try{
        Graphing grapher(width, height, fps, CsvLoader::load(csvPath));
        FFmpegPipe encoder(width, height, fps, outPath);

        cv::Mat frame;
        int totalFrames = (int)cap.get(cv::CAP_PROP_FRAME_COUNT);
        std::println("Rendering {} frames...", totalFrames);

        while (true) {
            cap >> frame;
            if (frame.empty()) break;
            int index = grapher.draw(frame);
            encoder.write(frame);
            if (index % 60 == 0) {
                std::print("\rEncoding: {:.1f}%", (double)index / totalFrames * 100.0);
            }
        }
        std::println("\nDone! Saved to {}", outPath);
    } catch(const std::exception& e){
        std::println(stderr, "Failed! Error: {}", e.what());
        return 1;
    }
    auto programTimeEnd = std::chrono::high_resolution_clock::now();
    auto programDuration = std::chrono::duration_cast<std::chrono::milliseconds>(programTimeEnd - programTimeStart).count();
    std::println("\nProgram took: {} ms", programDuration);
}
