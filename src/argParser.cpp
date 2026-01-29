#include "argParser.h"
#include <print>
#include <stdexcept>
#include <filesystem>

argParser::argParser(int argc, char* argv[], std::string mode)
: m_mode(mode)
{
    std::vector<std::string> posArgs;
    for (int i = 2; i < argc; i++){
        std::string arg(argv[i]);
        if(arg == "--threshold"){
            if(i + 1 < argc){
                a_config.threshold = std::stoi(argv[i + 1]);
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --threshold option needs a value (int)");
            }
        }
        if(arg == "--output"){
            if(i + 1 < argc){
                m_output = std::string(argv[i + 1]);
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --output option needs a value");
            }
        }
        if(arg == "--report"){
            a_config.report = true;
            continue;
        }
        if(arg == "--diffview"){
            a_config.diffView = true;
            continue;
        }
        if(arg == "--delay"){
            if(i + 1 < argc){
                a_config.delay = std::stoi(argv[i + 1]);
                a_config.delaySet = true;
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --delay option needs a value (int)");
            }
        }
        if(arg == "--tuning"){
            if(i + 1 < argc){
                a_config.tuning = std::stoi(argv[i + 1]);
                a_config.tuningSet = true;
                i++;
                continue;
            } else{
                throw std::runtime_error("Option Error: --tuning option needs a value (int)");
            }
        }
        if(arg == "--encoder"){
            if(i + 1 < argc){
                std::string val = argv[i + 1];
                if (val == "nvenc") v_config.encoder = EncoderType::NVENC;
                else if (val == "vce" || val == "amf") v_config.encoder = EncoderType::AMF;
                else v_config.encoder = EncoderType::CPU;
                i++; continue;
            } else{
                throw std::runtime_error("Option Error: --encoder option needs a value (encoder type)");
            }
        }
        posArgs.push_back(argv[i]);
    }
    posParse(posArgs);
    validate();
    confirmConfig();
}

void argParser::posParse(const std::vector<std::string>& posArgs){
    if(m_mode == "a"){
        if(posArgs.size() != 1){
            throw std::runtime_error("Error: too many positional arguments for analyzer mode (needs 1)");
        }
        a_config.inPath = posArgs[0];
        a_config.outPath = m_output;
    }
    else if(m_mode == "v"){
        if(posArgs.size() != 2){
            throw std::runtime_error("Error: too many positional arguments for visualizer mode (needs 2)");
        }
        v_config.videoPath = posArgs[0];
        v_config.csvPath = posArgs[1];
        v_config.outPath = m_output;
    }
    else if(m_mode == "av"){
        if(posArgs.size() != 2){
            throw std::runtime_error("Error: too many positional arguments for av mode (needs 2)");
        }
        a_config.inPath = posArgs[0];

        std::filesystem::path inPath(a_config.inPath);
        a_config.outPath = inPath.replace_extension("").string() + "-result.csv";
        v_config.csvPath = a_config.outPath;

        v_config.videoPath = posArgs[1];
        v_config.outPath = m_output;
    }
}

void argParser::validate(){
    if(m_mode == "a" || m_mode == "av"){
        if(a_config.inPath == ""){
            throw std::runtime_error("Input File Error: rias needs an input video to work on");
        }

        if(a_config.threshold < 0 || a_config.threshold > 255){
            throw std::runtime_error("Option Error: --threshold option value must be an int 0-255");
        }

        if(a_config.outPath != ""){
            std::filesystem::path outPath(a_config.outPath);
            std::string format = outPath.extension().string();
            if(format != ".csv"){
                throw std::runtime_error("Output File Error: rias analyzer can only output in csv format");
            }
        }else{
            std::filesystem::path inPath(a_config.inPath);
            a_config.outPath = inPath.replace_extension("").string() + "-result.csv";
        }

        if(!a_config.diffView && a_config.delaySet){
            std::println("--delay option supressed due to no --diffview flag. Proceeding with no diffview.");
        }

        if(a_config.delay < 0){
            a_config.delay = 1;
            std::println("Option Warning: --delay must be an int 0 or larger. Proceeding with default: 1");
        }

        if(a_config.tuning < 0){
            throw std::runtime_error("Option Error: --tuning option value must be between 0 and the recording fps");
        }
    }
    if(m_mode == "v" || m_mode == "av"){
        if(v_config.videoPath == ""){
            throw std::runtime_error("Input File Error: rias needs an input video to work on");
        }

        if(v_config.outPath != ""){
            std::filesystem::path outPath(v_config.outPath);
            std::string format = outPath.extension().string();
            if(format != ".mp4"){
                throw std::runtime_error("Output File Error: rias can only output in mp4 format");
            }
        }else{
            std::filesystem::path inPath(v_config.videoPath);
            v_config.outPath = inPath.replace_extension("").string() + "-result.mp4";
        }
    }
    if(a_config.tuningSet && m_mode != "a"){
        throw std::runtime_error("Mode error: tuning can only be done in analyzer mode");
    }
}

void argParser::confirmConfig(){
    if(m_mode == "a"){
        std::println("\nAnalysis file {} selected", a_config.inPath);
        std::println("Output file {} selected", a_config.outPath);
        std::println("Threshold {} selected", a_config.threshold);
        if(a_config.report){
            std::println("Report set to True");
        }
        if(a_config.diffView){
            std::println("DiffView set to {} with delay {}ms", a_config.diffView, a_config.delay);
        }
        if(a_config.tuningSet){
            std::println("Tuning mode starting with reference {}fps", a_config.tuning);
        }
    }
    else if(m_mode == "v"){
        std::println("\nVideo file {} selected", v_config.videoPath);
        std::println("Data csv file {} selected", v_config.csvPath);
        std::println("Output file {} selected", v_config.outPath);
        std::println("Encoder {} selected", etos(v_config.encoder));
    }
    else if(m_mode == "av"){
        std::println("\nAnalysis file {} selected", a_config.inPath);
        std::println("Video file {} selected", v_config.videoPath);
        std::println("Output file {} selected", v_config.outPath);
        std::println("Threshold for analysis {} selected", a_config.threshold);
        if(a_config.report){
            std::println("Report set to True");
        }
        if(a_config.diffView){
            std::println("DiffView set to {} with delay {}ms", a_config.diffView, a_config.delay);
        }
    }
}

std::string argParser::etos(EncoderType enc){
    if(enc == EncoderType::CPU){
        return "CPU";
    }
    else if(enc == EncoderType::NVENC){
        return "NVENC";
    }
    else if(enc == EncoderType::AMF){
        return "AMF";
    }
}

const AnalyzerConfig& argParser::a_getConfig() const{
    return a_config;
}

const VisualizerConfig& argParser::v_getConfig() const{
    return v_config;
}