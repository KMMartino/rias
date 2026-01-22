#include "ArgumentParser.h"
#include <print>
#include <stdexcept>
#include <filesystem>


ArgumentParser::ArgumentParser(int argc, char* argv[]){
    for (int i = 1; i < argc; i++){
        std::string arg(argv[i]);
        if(arg == "--threshold"){
            if(i + 1 < argc){
                m_config.threshold = std::stoi(argv[i + 1]);
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --threshold option needs a value (int)");
            }
        }
        if(arg == "--output"){
            if(i + 1 < argc){
                m_config.outPath = std::string(argv[i + 1]);
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --output option needs a value (filename.csv)");
            }
        }
        if(arg == "--report"){
            m_config.report = true;
            continue;
        }
        if(arg == "--diffview"){
            m_config.diffView = true;
            continue;
        }
        if(arg == "--mono"){
            m_config.mono = true;
            continue;
        }
        if(arg == "--delay"){
            if(i + 1 < argc){
                m_config.delay = std::stoi(argv[i + 1]);
                m_config.delaySet = true;
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --delay option needs a value (int)");
            }
        }
        if(arg == "--tuning"){
            if(i + 1 < argc){
                m_config.tuning = std::stoi(argv[i + 1]);
                m_config.tuningSet = true;
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --delay option needs a value (int)");
            }
        }
        m_config.inPath = argv[i];
    }
    validate();
    confirmConfig();
}


void ArgumentParser::validate(){
    if(m_config.inPath == ""){
        throw std::runtime_error("Input File Error: rias needs an input video to work on");
    }

    if(m_config.threshold < 0 || m_config.threshold > 255){
        throw std::runtime_error("Option Error: --threshold option value must be an int 0-255");
    }

    if(m_config.outPath != ""){
        std::filesystem::path outPath(m_config.outPath);
        std::string format = outPath.extension().string();
        if(format != ".csv"){
            throw std::runtime_error("Output File Error: rias can only output in csv format");
        }
    }else{
        std::filesystem::path inPath(m_config.inPath);
        m_config.outPath = inPath.replace_extension("").string() + "-result.csv";
    }

    if(!m_config.diffView && m_config.delaySet){
        std::println("--delay option supressed due to no --diffview flag. Proceeding with no diffview.");
    }

    if(m_config.delay < 0){
        m_config.delay = 1;
        std::println("Option Warning: --delay must be an int 0 or larger. Proceeding with default: 1");
    }

    if(m_config.tuning < 0){
        throw std::runtime_error("Option Error: --tuning option value must be between 0 and the recording fps");
    }
}

void ArgumentParser::confirmConfig(){
    std::println("\nInput file {} selected", m_config.inPath);
    std::println("Output file {} selected", m_config.outPath);
    std::println("Threshold {} selected", m_config.threshold);
    if(m_config.report){
        std::println("Report set to {}", m_config.report);
    }
    if(m_config.diffView){
        std::println("DiffView set to {} with delay {}ms", m_config.report, m_config.delay);
    }
    if(m_config.tuningSet){
        std::println("Tuning mode starting with reference {}fps", m_config.tuning);
    }
}

const riasConfig& ArgumentParser::getConfig() const {
    return m_config;
}