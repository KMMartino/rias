#pragma once
#include <string>
#include <vector>

struct AnalyzerConfig{
    int threshold = 20;
    std::string inPath = "";
    std::string outPath = "";
    bool report = false;
    bool diffView = false;
    int delay = 1;
    bool delaySet = false;
    int tuning = 60;
    bool tuningSet = false;
};

enum class EncoderType {
    CPU,
    NVENC,
    AMF
};

struct VisualizerConfig {
    std::string videoPath = "";
    std::string csvPath = "";
    std::string outPath = "";
    EncoderType encoder = EncoderType::CPU;
};

class argParser{
public:
    argParser(int argc, char* argv[], std::string mode);
    const AnalyzerConfig& a_getConfig() const;
    const VisualizerConfig& v_getConfig() const;
private:
    std::string m_mode;
    std::string m_output;
    AnalyzerConfig a_config;
    VisualizerConfig v_config;
    void posParse(const std::vector<std::string>& posArgs);
    void validate();
    void confirmConfig();
    std::string etos(EncoderType);
};