#pragma once
#include <string>
#include <vector>


class ArgumentParser{
public:
    ArgumentParser(int argc, char* argv[]);
    const int getThreshold();
    const std::string getInPath();
    const std::string getOutPath();
    const bool getReport();
    const bool getDiffView();
    const int getDelay();
private:
    int m_threshold = 30;
    std::string m_inPath = "";
    std::string m_outPath = "";
    bool m_report = false;
    bool m_diffView = false;
    int m_delay = 1;
};