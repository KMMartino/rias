# rias
This is "fRametIme anAlySis", or rias, a lightweight, CLI-based frametime / framerate analysis tool written in C++. 
Inspired by [trdrop](https://github.com/cirreth/trdrop), but stripped down for simple use. Tear detection is also not included.

This project is meant to double as a real world use tool and a boot.dev personal project. I got extensive assistance from AI in coding this but only for asking questions or checking usage of external libraries. All code was hand written. The core logic part got the most help but parts like the argument parsing and main are purely mine. 

## Requirements
* C++23 Compiler
* CMake 3.25+
* OpenCV (via vcpkg)

## Build
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=your_vcpkg.cmake_location
cmake --build build
```

## Usage
```bash
./rias.exe --flags filepath_to_input_video
```
This will output a csv file "filename-results.csv" containing 
Time(s), fps, frametime

## Flags
### --threshold (int 0-255, default: 30)
* Usage: --threshold int
* The amount that a grayscale pixel between frames can differ before it is counted as different, thus triggering a new frame. Higher values allow more noise before a new frame is triggered, thus lowering the framerate.
### --output (string xyz.csv, default: <input_file_name>-result.csv)
* Usage: --output string
* Sets the output file name. Probably the location too if you pass path/file.csv in. I'm not really sure. Also it will override files without notice (only csv). Be careful.
### --report
* Usage: --report
* Tells the program to print out a detailed report of things like 1% and 0.1% percentile frames, the number of major stutters, etc... Think of it like a --verbose flag for other programs. Also I thought it would be cool to implement a sorting algorithm from scratch in C++. Since boot.dev made me do it in python. Nevemind. I ended up using a map rather than a proper sorting algorithm. 
### --diffview
* Usage --diffview
* Opens a window to display the frame diffs (changed pixels shown white. Everything else black). If the same frame is shown more than once, it replays the last diff but in yellow, then red for any more. This process depends on your PC spec and can be very fast and impossible to decipher. Use with the following --delay option to mitigate this.
### --delay (int >= 0, default: 1)
* Usage --delay int
* Use with the above --diffview; adds a delay to displaying the diff frame to make the output more readable. The delay is in ms. Selecting 0 will enter a different mode where the program will not proceed to processing the next frame until user input(spacebar). Holding the input will make the program proceed as normal. 

## Framerate calculation quirks
* Due to the rolling framerate calculation logic and the program not having a full 1 sec worth of frames to work with at the beginning, the framerate numbers for the first 1 sec should be ignored
* There has been a framerate extrapolator implemented for the first 1 sec which will give you a rough estimate of framerate during those times, but this is only a rough estimate especially for the earlier frames.
* Similarly the last 1 sec of the frames will never be touched (todo: update logic to account for these situations)

## How it works
* rias takes 2 frames, compares their grayscales and counts up the number of pixels that are more different than the threshold. If it counts any, it will count the frames as different and record a short frametime / high framerate. 
* If it detects that the frames are similar enough, it will record a long frametime / low framerate.

## Todo
* Add --tuning option
    * Input pass in user hand counted framerate so the program can tell you if your threshold is set correctly.
* Add framerate visualization / graphing tools.
* Optimize usage of opencv libraries to improve performance.