#include <opencv2/opencv.hpp>
#include "analyzer.h"
#include <print>
#include <filesystem>
#include "ArgumentParser.h"

int main(int argc, char** argv) {
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
        
    } catch(const std::exception& e){
        std::println("Error processing arguments: {}", e.what());
        return 1;
    }
}