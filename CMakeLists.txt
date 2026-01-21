cmake_minimum_required(VERSION 3.25)
set(CMAKE_TOOLCHAIN_FILE "C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(rias LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_library(rias_common STATIC
    src/common/ArgumentParser.cpp
)
target_include_directories(rias_common PUBLIC src/common)

add_executable(rias-analyzer 
    src/analyzer/main.cpp
    src/analyzer/Analyzer.cpp
    src/analyzer/FrameProcessor.cpp
)
target_include_directories(rias-analyzer PRIVATE src/analyzer)
target_link_libraries(rias-analyzer PRIVATE rias_common ${OpenCV_LIBS})

add_executable(rias-visualizer 
    src/visualizer/main.cpp
)
target_include_directories(rias-visualizer PRIVATE src/visualizer)
target_link_libraries(rias-visualizer PRIVATE rias_common ${OpenCV_LIBS})

if(MSVC)
    target_compile_options(rias_common PRIVATE /W4)
else()
    target_compile_options(rias_common PRIVATE -Wall -Wextra)
endif()