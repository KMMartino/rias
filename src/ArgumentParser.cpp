#include "ArgumentParser.h"
#include <print>
#include <stdexcept>
#include <filesystem>


ArgumentParser::ArgumentParser(int argc, char* argv[]){
    for (int i = 1; i < argc; i++){
        std::string arg(argv[i]);
        if(arg == "--threshold"){
            if(i + 1 < argc){
                m_threshold = std::stoi(argv[i + 1]);
                std::println("Threshold {} selected", m_threshold);
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --threshold option needs a value (int)");
            }
        }
        if(arg == "--output"){
            if(i + 1 < argc){
                m_outPath = std::string(argv[i + 1]);
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --output option needs a value (filename.csv)");
            }
        }
        m_inPath = argv[i];
    }
    if(m_inPath == ""){
        throw std::runtime_error("rias needs an input video to work on");
    }
    std::println("Input file {} selected", m_inPath);

    if(m_outPath != ""){
        std::filesystem::path outPath(m_outPath);
        std::string format = outPath.extension().string();
        if(format != ".csv"){
            throw std::runtime_error("Output File Error: rias can only output in csv format");
        }
    }else{
        std::filesystem::path inPath(m_inPath);
        m_outPath = inPath.replace_extension("").string() + "-result.csv";
    }
    std::println("Output file {} selected", m_outPath);
}


int ArgumentParser::getThreshold(){
    return m_threshold;
}

std::string ArgumentParser::getInPath(){
    return m_inPath;
}

std::string ArgumentParser::getOutPath(){
    return m_outPath;
}