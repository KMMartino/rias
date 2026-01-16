cmake_minimum_required(VERSION 3.25)

# --- ADD THIS LINE AT THE TOP ---
set(CMAKE_TOOLCHAIN_FILE "C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake")
# --------------------------------

project(rias LANGUAGES CXX)

# Specify C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find OpenCV
find_package(OpenCV REQUIRED)

add_executable(rias src/main.cpp)

# Link OpenCV
target_link_libraries(rias PRIVATE ${OpenCV_LIBS})

if(MSVC)
    target_compile_options(rias PRIVATE /W4)
else()
    target_compile_options(rias PRIVATE -Wall -Wextra)
endif()