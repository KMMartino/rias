#pragma once
#include <string>
#include <vector>

enum class EncoderType {
    CPU,
    NVENC,
    AMF
};

struct visualizerConfig{
    std::string videoPath = "";
    std::string csvPath = "";
    std::string outPath = "";
    EncoderType encoder = EncoderType::CPU;
    bool mono = false;
};

class v_ArgumentParser{
public:
    v_ArgumentParser(int argc, char* argv[]);
    const visualizerConfig& getConfig() const;
private:
    visualizerConfig m_config;
    void validate();
    void confirmConfig();
};