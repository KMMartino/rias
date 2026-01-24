#pragma once
#include <string>
#include <vector>

struct visualizerConfig{
    std::string videoPath = "";
    std::string csvPath = "";
    std::string outPath = "";
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