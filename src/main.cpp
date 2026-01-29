#include <print>
#include <filesystem>
#include "analyzer/AnalyzerEntry.h"
#include "visualizer/VisualizerEntry.h"
#include "argParser.h"

void printUsage() {
    std::println("Usage:");
    std::println("  rias a --Options <input_video>");
    std::println("  rias v --Options <input_video> <input_csv>");
    std::println("  rias av --Options <analysis_video> <render_video>");
}

int main(int argc, char** argv) {
    if (argc < 3) {
        printUsage();
        return 1;
    }
    std::string mode = argv[1];
    try{
        argParser parser(argc, argv, mode);
        if(mode == "a"){
            AnalyzerConfig a_config = parser.a_getConfig();
            return runAnalysis(a_config);
        }
        else if(mode == "v"){
            VisualizerConfig v_config = parser.v_getConfig();
            return runVisualizer(v_config);
        }
        else if(mode == "av"){
            std::println("[Step 1/2] Analyzing Performance Data...");
            AnalyzerConfig a_config = parser.a_getConfig();
            if(runAnalysis(a_config) != 0 ) return 1;

            std::println("\n[Step 2/2] Rendering Visualization...");
            VisualizerConfig v_config = parser.v_getConfig();
            int result = runVisualizer(v_config);

            std::filesystem::remove(a_config.outPath);
            std::println("Pipeline Complete!");
            return result;
        }
        else{
            throw std::runtime_error("Error: invalid mode selection. Options are: a, v, av");
        }
    } catch(const std::exception& e){
        std::println(stderr, "Failed! Error: {}", e.what());
        return 1;
    }
}