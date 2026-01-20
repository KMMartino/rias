#pragma once
#include <string>
#include <vector>

struct riasConfig{
    int threshold = 30;
    std::string inPath = "";
    std::string outPath = "";
    bool report = false;
    bool diffView = false;
    int delay = 1;
    bool delaySet = false;
    int tuning = 60;
    bool tuningSet = false;
};

class ArgumentParser{
public:
    ArgumentParser(int argc, char* argv[]);
    const riasConfig& getConfig() const;
private:
    riasConfig m_config;
    void validate();
    void confirmConfig();
};