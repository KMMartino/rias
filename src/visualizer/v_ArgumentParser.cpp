#include "v_ArgumentParser.h"
#include <print>
#include <stdexcept>
#include <filesystem>


v_ArgumentParser::v_ArgumentParser(int argc, char* argv[]){
    std::vector<std::string> posArgs;
    for (int i = 1; i < argc; i++){
        std::string arg(argv[i]);
        if(arg == "--output"){
            if(i + 1 < argc){
                m_config.outPath = std::string(argv[i + 1]);
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --output option needs a value (filename.csv)");
            }
        }
        if(arg == "--encoder"){
            if(i + 1 < argc){
                std::string val = argv[i + 1];
                if (val == "nvenc") m_config.encoder = EncoderType::NVENC;
                else if (val == "vce" || val == "amf") m_config.encoder = EncoderType::AMF;
                else m_config.encoder = EncoderType::CPU;
                i++; continue;
            } else{
                throw std::runtime_error("Option Error: --output option needs a value (filename.csv)");
            }
        }
        if(arg == "--mono"){
            m_config.mono = true;
            continue;
        }
        posArgs.push_back(argv[i]);
    }
    if(posArgs.size() != 2){
        throw std::runtime_error("Error: rias visualizer requires 2 positional arguments");
    }
    m_config.videoPath = posArgs[0];
    m_config.csvPath = posArgs[1];
    validate();
    confirmConfig();
}

void v_ArgumentParser::validate(){
    if(m_config.videoPath == ""){
        throw std::runtime_error("Input File Error: rias needs an input video to work on");
    }

    if(m_config.outPath != ""){
        std::filesystem::path outPath(m_config.outPath);
        std::string format = outPath.extension().string();
        if(format != ".mp4"){
            throw std::runtime_error("Output File Error: rias can only output in mp4 format");
        }
    }else{
        std::filesystem::path inPath(m_config.videoPath);
        m_config.outPath = inPath.replace_extension("").string() + "-result.mp4";
    }
}

void v_ArgumentParser::confirmConfig(){
    std::println("\nInput file {} selected", m_config.videoPath);
    std::println("Output file {} selected", m_config.outPath);
    std::println("csv data {} selected", m_config.csvPath);
}

const visualizerConfig& v_ArgumentParser::getConfig() const {
    return m_config;
}