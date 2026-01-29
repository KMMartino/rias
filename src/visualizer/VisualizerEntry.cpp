#include "VisualizerEntry.h"
#include "Renderer.h"

int runVisualizer(const VisualizerConfig& config){
    Renderer renderer(config);
    renderer.run();
    return 0;
}
