#include <opencv2/opencv.hpp>
#include "analyzer.h"
#include <print>
#include <filesystem>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::println("Usage: rias.exe <video_path>");
        return 1;
    }

    std::string videoPath = argv[1];
    std::filesystem::path filePath(videoPath);
    std::string csvPath = filePath.replace_extension("").string() + "-results.csv";

    Analyzer analyzer;
    if (analyzer.analyze(videoPath)) {
        analyzer.exportCsv(csvPath);
    }
}