#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <print>

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
        std::ifstream file(path);
        
        if (!file.is_open()) {
            std::println(stderr, "Error: Could not open CSV {}", path);
            return data;
        }

        std::string line;
        std::getline(file, line); 

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string cell;
            FrameData frame;

            // Helper to parse comma-separated doubles
            auto getNext = [&](double& val) {
                if (std::getline(ss, cell, ',')) val = std::stod(cell);
            };

            getNext(frame.timestamp);
            getNext(frame.fpsTotal);
            getNext(frame.fpsCurrent);
            getNext(frame.frametime);
            
            data.push_back(frame);
        }
        std::println("Loaded {} data points.", data.size());
        return data;
    }
};