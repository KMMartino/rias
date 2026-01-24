#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <print>
#include <cstdlib>

struct FrameData {
    double timestamp;
    double fpsTotal;
    double fpsCurrent;
    double frametime;
};

class CsvLoader {
public:
    static std::vector<FrameData> load(const std::string& path) {
        std::vector<FrameData> data;
        data.reserve(10000);
        std::ifstream file(path);
        
        if (!file.is_open()) {
            std::println(stderr, "Error: Could not open CSV {}", path);
            return data;
        }

        std::string line;
        std::getline(file, line); 
        char* endPtr;
        const char* linePtr;

        while (std::getline(file, line)) {
            FrameData frame;
            linePtr = line.c_str();

            frame.timestamp = std::strtod(linePtr, &endPtr);
            frame.fpsTotal = std::strtod(endPtr + 1, &endPtr);
            frame.fpsCurrent = std::strtod(endPtr + 1, &endPtr);
            frame.frametime = std::strtod(endPtr + 1, &endPtr);
            
            data.push_back(frame);
        }
        std::println("Loaded {} data points.", data.size());
        return data;
    }
};