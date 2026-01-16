# rias
A lightweight, CLI-based framerate analysis tool written in C++. 
Based on the logic of [trdrop](https://github.com/cirreth/trdrop), but stripped down for backend analysis without tear detection.

## Requirements
* C++23 Compiler
* CMake 3.25+
* OpenCV (via vcpkg)

## Build
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

## Usage
```bash
./rias.exe <filepath to video>
```
This will output a csv file "filename-results.csv" containing 
Time(s), fps, frametime
