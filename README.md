# rias-V3
This is "fRametIme anAlySis", or rias, a lightweight, CLI-based frametime / framerate analysis tool written in C++. 
Inspired by [trdrop](https://github.com/cirreth/trdrop), but stripped down for simple use. Tear detection is also not included.

V2 adds a second exe to the project, in charge of graphing and putting the graph into the video. rias also from now on assumes a specific video format / structure with 2 files.
1. full resolution video file with compression
2. small video encoded losslessly with the same frames as 1 (modulo being off by a few frames at beginning and end)
rias-analyzer takes file 2 to generate a csv containing all necessary frame data and passes it to rias-visualizer to bake the graphs into based on the csv into video 1. Option to output graphs with transparent background may be coming soon. Who knows.

Once done the default will be no resizing video feed to optimize performance since rias assumes you have a small video. --mono flag also in the works for people who just have a single full res lossless video file to analyze and bake in graph.

Test files included in files as example of the 2 video analysis structure. fr is full res and ll is lossless. They are a total of 600MB or so. The original concept with a high res lossless file would have probably required a 5-10 GB video file. I say the complex structure of the project is worth the over 10x reduction in video storage size.

## Requirements
- C++23 compiler
- CMake 3.25+
- OpenCV 4.x (via vcpkg)
  - Required modules: ffmpeg, freetype, contrib
- FFmpeg in your PATH (the visualizer opens a pipe to ffmpeg)
- Windows: a font at C:/Windows/Fonts/arialbd.ttf (falls back to arial.ttf). If those aren’t present, text rendering may not show.

### Installing Dependencies (vcpkg)
```powershell
vcpkg install "opencv[ffmpeg,freetype,contrib]:x64-windows" --recurse
```

## Build
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config release
```
Note: the toolchain file path may differ on your system. Also the top-level CMake may already set a default vcpkg path; override it if needed.

## Usage
**You need Lossless video for this to work!!**
The program expects a mode as the first positional:
- Analyzer mode (a):
  ```bash
  rias a [options] <input_video>
  ```
- Visualizer mode (v):
  ```bash
  rias v [options] <input_video> <input_csv>
  ```
- Combined mode (av): analyze then render:
  ```bash
  rias av [options] <analysis_video> <render_video>
  ```

### What each mode does
- a: Runs the analysis on a small lossless video and writes a CSV. Intended to be the “data pass”.
- v: Reads a high-res (but doesn't have to be lossless) video and a CSV, draws graphs, and encodes an output MP4 (optionally GPU-accelerated).
- av: Combined pipeline. Analyzes <analysis_video>, writes a temporary CSV, then visualizes onto <render_video>. The temporary CSV is deleted at the end of the run.

### Assumed structure
- It is assumed that you have a high resolution lossy video and a low resolution lossless video recorded simultaneously.
- The frametime analysis is done based on the lossless file and the nice looking high res video with the overlay is based on the high res video.
- This simultaneous recording is normally done using the obs source record plugin.
- The plugin produces a recording that is most likely a frame or two off so rias-visualizer includes (in the future) logic to sync the video streams.
- While using a single high resolution lossless video is possible, it is not recommended due to storage space inefficiency. This type of usage will be done with the --mono flag (when implemented). For now, just pass the same video twice.

## Flags (rias-analyzer; mode a and av)
### --threshold (int 0-255, default: 20, *flag parameter required*)
- Usage: --threshold int
- The amount that a grayscale pixel between frames can differ before it is counted as different, thus triggering a new frame. Higher values allow more noise before a new frame is triggered, thus lowering the framerate.
- The default is set lower than in V1 because this version assumes a lower resolution base video and that seems to be harder to detect new frames at higher thresholds. Though to be honest as long as the input video is properly lossless very low threshold numbers should be fine. Be sure to raise the threshold value if you're using the --mono flag with a larger video and make good use of the --tuning feature.
### --output (string xyz.csv, default: <input_file_name>-result.csv, *flag parameter required*)
- Usage: --output string
- Sets the output file name. Probably the location too if you pass path/file.csv in. I'm not really sure. Also it will override files without notice (only csv). Be careful.
### --report (*no flag parameter*)
- Usage: --report
- Tells the program to print out a detailed report of things like 1% and 0.1% percentile frames, the number of major stutters, etc... Think of it like a --verbose flag for other programs. Also I thought it would be cool to implement a sorting algorithm from scratch in C++. Since boot.dev made me do it in python. Nevemind. I ended up using a map rather than a proper sorting algorithm. 
### --diffview (*no flag parameter*)
- Usage --diffview
- Opens a window to display the frame diffs (pixel diffs shown in BGR. Everything else black). If the same frame is shown more than once, it replays the last diff but in yellow, then red for any more. This process depends on your PC spec and can be very fast and impossible to decipher. Use with the following --delay option to mitigate this.
- Compared to the standard operations this will significantly hurt performance.
### --delay (int >= 0, default: 1, *flag parameter required*)
- Usage --delay int
- Use with the above --diffview; adds a delay to displaying the diff frame to make the output more readable. The delay is in ms. Selecting 0 will enter a different mode where the program will not proceed to processing the next frame until user input(spacebar). Holding the input will make the program proceed as normal. 
- Selecting a high delay will make the program take for ever to finish executing. You can press esc to exit early. You may have to mash it a bit. It's a bit janky.
### --tuning (int 0-videofps, no default, *flag parameter required*)
- Usage --tuning int
- Runs the program in a mode where it only counts the first 1 sec worth of frames, and compares the number to the provided flag parameter. It will recommend a different --threshold to use based on the results.
- Best used with the --threshold option to test different threshold values and make sure the framerate counting is accurate.
- **Important** If you use lossless video as the input the default threshold should be perfectly fine. While the tuning option is here, numbers produced by this program for a lossy input file, even if tuned, should not be considered reliable.

## Flags (rias-visualizer; mode v and av)
### --output (string xyz.csv, default: <input_file_name>-result.csv, *flag parameter required*)
- Usage: --output string
- Sets the output file name. Probably the location too if you pass path/file.csv in. I'm not really sure. Also it will override files without notice (only mp4). Be careful.
### --encoder (string option, default: cpu, *flag parameter required*)
- Usage --encoder option
- Options are cpu, nvenc, amf, and vce. The last 2 are the same thing.
- This allows GPU encoding for the output video.

## Flags (rias; mode av)
### --offset (int 0-n, default: auto detect, *flag parameter required*)
- Usage --offset int
- It is likely that the lossless video will have the first few frames of the full res video missing due to OBS source record not being perfect. The program will auto detect this difference and apply that but you can also set it manually. 
- --offset -1 was going to be reserved so you can run in av mode to get the offset then pass that into v mode as the offset if you happened to have the csv.
- This idea was ditched because a mode runs so much faster than v mode that if you're going through the process of running av with offset -1 then going back to v mode with offset, just run av mode normally. That should be a lot quicker.
### --mono (*no flag parameter*)
- Usage --mono
- In case you have a high res lossless file.
- This will allow you to pass a single video file to av mode and have it still work. It will also adjust flags and such in the background to make everything work.

## Framerate calculation quirks
- Due to the rolling framerate calculation logic and the program not having a full 1 sec worth of frames to work with at the beginning, the framerate numbers for the first 1 sec should be ignored
- There has been a framerate extrapolator implemented for the first 1 sec which will give you a rough estimate of framerate during those times, but this is only a rough estimate especially for the earlier frames.
- Similarly the last 1 sec of the frames will never be touched (todo: update logic to account for these situations)

## How it works
### Analyzer
- rias takes 2 frames, compares their green channel and counts up the number of pixels that are more different than the threshold. If it counts any, it will count the frames as different and record a short frametime / high framerate. 
- If it detects that the frames are similar enough, it will record a long frametime / low framerate.
### Visualizer
- rias will take the csv data to create a framerate and frametime plot, then draw those onto the video using opencv.
- The video is encoded into an mp4 via ffmpeg.

## Project status / notes
- This is a personal tool; I don't know how github works really; Stuff may break.

## Todo
- Add other encoding options.
- Add flag to not delete csv in av mode.