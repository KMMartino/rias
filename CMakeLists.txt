cmake_minimum_required(VERSION 3.25)
set(CMAKE_TOOLCHAIN_FILE "C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(rias LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

find_package(OpenCV REQUIRED)

add_library(rias-analyzer-lib STATIC
    src/analyzer/Analyzer.cpp
    src/analyzer/Analyzer.h
    src/analyzer/FrameProcessor.cpp
    src/analyzer/FrameProcessor.h
    src/analyzer/AnalyzerEntry.h
    src/analyzer/AnalyzerEntry.cpp
)
target_include_directories(rias-analyzer-lib PUBLIC src src/analyzer)
target_link_libraries(rias-analyzer-lib PRIVATE ${OpenCV_LIBS})

add_library(rias-visualizer-lib STATIC
    src/visualizer/Renderer.h
    src/visualizer/Renderer.cpp
    src/visualizer/Graphing.cpp
    src/visualizer/Graphing.h
    src/visualizer/Plot.h
    src/visualizer/Plot.cpp
    src/visualizer/CsvLoader.h
    src/visualizer/FFmpegPipe.h
    src/visualizer/FFmpegPipe.cpp
    src/visualizer/VisualizerEntry.h
    src/visualizer/VisualizerEntry.cpp
)
target_include_directories(rias-visualizer-lib PUBLIC src src/visualizer)
target_link_libraries(rias-visualizer-lib PRIVATE ${OpenCV_LIBS})

add_executable(rias
    src/main.cpp
    src/argParser.cpp
    src/argParser.h
)

target_link_libraries(rias PRIVATE 
    rias-analyzer-lib 
    rias-visualizer-lib 
    ${OpenCV_LIBS}
)

target_include_directories(rias PRIVATE 
    src 
    src/analyzer 
    src/visualizer
)
