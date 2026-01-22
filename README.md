# rias-V2 - 0.0.1
This is "fRametIme anAlySis", or rias, a lightweight, CLI-based frametime / framerate analysis tool written in C++. 
Inspired by [trdrop](https://github.com/cirreth/trdrop), but stripped down for simple use. Tear detection is also not included.

This project is meant to double as a real world use tool and a boot.dev personal project. I got extensive assistance from AI in coding this but only for asking questions or checking usage of external libraries. All code was hand written. The core logic part got the most help but parts like the argument parsing and main are purely mine. 

V2 adds a second exe to the project, in charge of graphing and putting the graph into the video. rias also from now on assumes a specific video format / structure with 2 files.
1. full resolution video file with compression
2. small video encoded losslessly with the same frames as 1 (modulo being off by a few frames at beginning and end)
rias-analyzer takes file 2 to generate a csv containing all necessary frame data and passes it to rias-visualizer to bake the graphs into based on the csv into video 1. Option to output graphs with transparent background may be coming soon. Who knows.

Once done the default will be no resizing video feed to optimize performance since rias assumes you have a small video. --mono flag also in the works for people who just have a single full res lossless video file to analyze and bake in graph.

Test files included in files as example of the 2 video analysis structure. fr is full res and ll is lossless. They are a total of 600MB or so. The original concept with a high res lossless file would have probably required a 5-10 GB video file. I say the complex structure of the project is worth the over 10x reduction in video storage size.

## Requirements
* C++23 Compiler
* CMake 3.25+
* OpenCV 4.x (via vcpkg)
    * *Required Modules:* `ffmpeg`, `freetype`, `contrib`
### Installing Dependencies (vcpkg)
```powershell
vcpkg install "opencv[ffmpeg,freetype,contrib]:x64-windows" --recurse
```
## Build
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config release
```

## Usage

**You need Lossless video for this to work!!**

```bash
./rias-analyzer.exe --flags filepath_to_input_video
```
This will output a csv file "filename-results.csv" containing 
Time(s), fps, frametime(ms), unique(bool)
```bash
./rias-visualizer.exe filepath_to_input_video filepath_to_csv_file
```
This takes in an input video and expoted csv file and placed a graph based on the csv on the input video. It is assumed that you have a high resolution lossy video and a low resolution lossless video recorded simultaneously. The frametime analysis is done based on the lossless file and the nice looking high res video with the overlay is based on the high res video. This simultaneous recording is normally done using the obs source record plugin. The plugin produces a recording that is a frame or two off so rias-visualizer includes logic to sync the video streams. While using a single high resolution lossless video is possible, it is not recommended due to storage space inefficiency.

## Flags (rias-analyzer)
### --threshold (int 0-255, default: 20, *flag parameter required*)
* Usage: --threshold int
* The amount that a grayscale pixel between frames can differ before it is counted as different, thus triggering a new frame. Higher values allow more noise before a new frame is triggered, thus lowering the framerate.
* The default is set lower than in V1 because this version assumes a lower resolution base video and that seems to be harder to detect new frames at higher thresholds. Though to be honest as long as the input video is properly lossless very low threshold numbers should be fine. Be sure to raise the threshold value if you're using the --mono flag with a larger video and make good use of the --tuning feature.
### --output (string xyz.csv, default: <input_file_name>-result.csv, *flag parameter required*)
* Usage: --output string
* Sets the output file name. Probably the location too if you pass path/file.csv in. I'm not really sure. Also it will override files without notice (only csv). Be careful.
### --report (*no flag parameter*)
* Usage: --report
* Tells the program to print out a detailed report of things like 1% and 0.1% percentile frames, the number of major stutters, etc... Think of it like a --verbose flag for other programs. Also I thought it would be cool to implement a sorting algorithm from scratch in C++. Since boot.dev made me do it in python. Nevemind. I ended up using a map rather than a proper sorting algorithm. 
### --diffview (*no flag parameter*)
* Usage --diffview
* Opens a window to display the frame diffs (pixel diffs shown in BGR. Everything else black). If the same frame is shown more than once, it replays the last diff but in yellow, then red for any more. This process depends on your PC spec and can be very fast and impossible to decipher. Use with the following --delay option to mitigate this.
* Compared to the standard operations this will significantly hurt performance.
### --delay (int >= 0, default: 1, *flag parameter required*)
* Usage --delay int
* Use with the above --diffview; adds a delay to displaying the diff frame to make the output more readable. The delay is in ms. Selecting 0 will enter a different mode where the program will not proceed to processing the next frame until user input(spacebar). Holding the input will make the program proceed as normal. 
* Selecting a high delay will make the program take for ever to finish executing. You can press esc to exit early. You may have to mash it a bit. It's a bit janky.
### --tuning (int 0-videofps, no default, *flag parameter required*)
* Usage --tuning int
* Runs the program in a mode where it only counts the first 1 sec worth of frames, and compares the number to the provided flag parameter. It will recommend a different --threshold to use based on the results.
* Best used with the --threshold option to test different threshold values and make sure the framerate counting is accurate.
* **Important** If you use lossless video as the input the default threshold should be perfectly fine. While the tuning option is here, numbers produced by this program for a lossy input file, even if tuned, should not be considered reliable.
### --mono (*no flag parameter*)
* Usage --mono
* V2 of rias assumes that you have a high res lossy file and a low res lossless file. This will be the default moving forward, but --mono is here in case you have a high res lossless file.

## Flags (rias-visualizer)
### ---

## Framerate calculation quirks
* Due to the rolling framerate calculation logic and the program not having a full 1 sec worth of frames to work with at the beginning, the framerate numbers for the first 1 sec should be ignored
* There has been a framerate extrapolator implemented for the first 1 sec which will give you a rough estimate of framerate during those times, but this is only a rough estimate especially for the earlier frames.
* Similarly the last 1 sec of the frames will never be touched (todo: update logic to account for these situations)

## How it works
* rias takes 2 frames, compares their green channel and counts up the number of pixels that are more different than the threshold. If it counts any, it will count the frames as different and record a short frametime / high framerate. 
* If it detects that the frames are similar enough, it will record a long frametime / low framerate.

## Todo
* Major: Add framerate visualization / graphing tools.
* Add tool to automatically sync analysis video and full quality video.
* Add a flag --mono to switch between intended mode and single input mode.
* Add support for 30 and 120fps video (currently assumes 60fps recording)