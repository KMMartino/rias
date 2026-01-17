#include <opencv2/opencv.hpp>
#include "analyzer.h"
#include <print>
#include <filesystem>
#include "ArgumentParser.h"
#include <chrono>

int main(int argc, char** argv) {
    auto programTimeStart = std::chrono::high_resolution_clock::now();
    if (argc < 2) {
        std::println("Usage: rias.exe --flags <video_path>");
        return 1;
    }

    try{
        ArgumentParser parser(argc, argv);
        Analyzer analyzer(parser.getThreshold());
        if (analyzer.analyze(parser.getInPath())) {
            analyzer.exportCsv(parser.getOutPath());
        }
        auto programTimeEnd = std::chrono::high_resolution_clock::now();
        auto loopTimeEnd = std::chrono::high_resolution_clock::now();
        auto programDuration = std::chrono::duration_cast<std::chrono::milliseconds>(programTimeEnd - programTimeStart).count();
        std::println("\nProgram took: {} ms", programDuration);
        
    } catch(const std::exception& e){
        std::println("Error processing arguments: {}", e.what());
        return 1;
    }
}