#include <print>
#include <deque>
#include <opencv2/opencv.hpp>
#include <opencv2/freetype.hpp>
#include "FFmpegPipe.h"
#include "Graphing.h"
#include <chrono>
#include "v_ArgumentParser.h"
#include "Renderer.h"


int main(int argc, char** argv){
    auto programTimeStart = std::chrono::high_resolution_clock::now();
    if (argc < 3) {
        std::println("Usage: rias-visualizer.exe <video_path> <csv_path>");
        return 1;
    }
    try{
        v_ArgumentParser parser(argc, argv);
        visualizerConfig config = parser.getConfig();
        Renderer renderer(config);
        renderer.run();
    } catch(const std::exception& e){
        std::println(stderr, "Failed! Error: {}", e.what());
        return 1;
    }
    auto programTimeEnd = std::chrono::high_resolution_clock::now();
    auto programDuration = std::chrono::duration_cast<std::chrono::milliseconds>(programTimeEnd - programTimeStart).count();
    std::println("\nProgram took: {} ms", programDuration);
}
