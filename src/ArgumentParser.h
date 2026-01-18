#pragma once
#include <string>
#include <vector>


class ArgumentParser{
public:
    ArgumentParser(int argc, char* argv[]);
    int getThreshold();
    std::string getInPath();
    std::string getOutPath();
    bool getReport();
private:
    int m_threshold = 30;
    std::string m_inPath = "";
    std::string m_outPath = "";
    bool m_report = false;
};