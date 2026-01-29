# rias-V2 (single binary)
This is "fRametIme anAlySis", or rias, a lightweight, CLI-based frametime / framerate analysis tool written in C++.

V2 used to ship as two executables, but after a refactor it’s now a single binary with modes. The analyzer logic and the visualizer live as internal libs and the main just dispatches by mode. Functionally it’s the same idea: analyze a lossless video → bake graphs onto your nice looking video.

I still consider this a personal project. I got a lot of help from AI for checking library usage and asking questions, but I hand-wrote the code. Expect rough edges and me changing stuff as I learn.

## Requirements
- C++23 compiler
- CMake 3.25+
- OpenCV 4.x (via vcpkg)
  - Required modules: ffmpeg, freetype, contrib
- FFmpeg in your PATH (the visualizer opens a pipe to ffmpeg)
- Windows: a font at C:/Windows/Fonts/arialbd.ttf (falls back to arial.ttf). If those aren’t present, text rendering may not show.

### Installing dependencies (vcpkg)
```powershell
vcpkg install "opencv[ffmpeg,freetype,contrib]:x64-windows" --recurse
```

## Build
```bash
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config release
```
Note: the toolchain file path may differ on your system. Also the top-level CMake may already set a default vcpkg path; override it if needed.

## Usage (single binary with modes)
The program expects a mode as the first positional:
- Analyzer mode (a):
  ```bash
  rias a [options] <input_video>
  ```
- Visualizer mode (v):
  ```bash
  rias v [options] <input_video> <input_csv>
  ```
- Pipeline mode (av): analyze then render:
  ```bash
  rias av [options] <analysis_video> <render_video>
  ```

### What each mode does
- a: Runs the analysis on a small lossless video and writes a CSV. Intended to be the “data pass”.
- v: Reads a high-res (nice looking) video and a CSV, draws graphs, and encodes an output MP4 (optionally GPU-accelerated).
- av: Convenience pipeline. Analyzes <analysis_video>, writes a temporary CSV, then visualizes onto <render_video>. The temporary CSV is deleted at the end of the run.

## Options
Some options only apply to certain modes. If you pass the wrong combo, the app errors out.

### Analyzer options (mode: a, and analysis step of av)
- --threshold int (0-255, default: 20)
  - How different a pixel (green channel) must be to count as a new frame. Higher = allow more noise before counting as a new frame (so lower measured FPS).
- --output path\file.csv (default: <input_file_name>-result.csv)
  - CSV output path. Will overwrite silently if it’s a .csv. In av mode this is auto-set from the analysis input.
- --report (flag)
  - Print a detailed report: percentiles (1%/0.1%), stutter counts, etc.
- --diffview (flag)
  - Show a preview window of frame diffs (BGR). Duplicates are highlighted (first duplicate yellow-ish, then red). Slows down processing a lot.
- --delay int (>=0, default: 1)
  - Only used with --diffview. Delay in ms between frames. 0 enters a step-through mode where you can hold space to run.
- --tuning int (0–video_fps)
  - Analyze only ~1 second worth of frames and compare the counted unique frames to your provided reference. Tells you if you should adjust --threshold up or down.
  - Only valid in analyzer mode.

### Visualizer options (mode: v, and visualize step of av)
- --output path\file.mp4 (default: <input_file_name>-result.mp4)
  - MP4 output path. Will overwrite silently if it’s a .mp4.
- --encoder option (default: cpu)
  - Options: cpu, nvenc, amf (or vce). cpu uses libx264; nvenc uses h264_nvenc; amf uses h264_amf.
  - Requires FFmpeg to be in PATH and your GPU encoder to be supported.

## CSV format
The analyzer currently writes a CSV with header:
```
Time(s),fps(total),fps(current),Frametime(ms),Unique(bool)
```
- The visualizer uses the first four columns; the Unique(bool) column is ignored there.
- The initial second of data is extrapolated; the very end may be incomplete due to the rolling-window logic.

## How it works (short version)
- Analyzer: downscales frames, compares the green channel, thresholds the difference, and marks a frame as unique if there’s any non-zero above threshold. Unique frames determine frametime and FPS.
- Visualizer: loads the CSV, draws FPS and frametime plots on top of your high-res video using OpenCV (with freetype text), and streams raw frames to FFmpeg for encoding.

## Framerate calculation quirks
- The first ~1 second doesn’t have a full rolling window, so the numbers are estimates.
- Similarly, the final ~1 second will have less stable numbers.

## Recommended recording setup
- Two files:
  1) Full-resolution lossy video for presentation.
  2) Small resolution, lossless video for analysis (same frames, maybe off by a couple at start/end).
- Record both simultaneously (e.g., OBS source record). v-mode assumes the CSV matches the render video timeline closely enough.

## Project status / notes
- This is a personal tool; expect changes. If something breaks after an update (OpenCV/FFmpeg/etc.), it’s probably me relying on transitive includes or assumptions. I’m trying to be better about that.

## Todo (moving target)
- Better auto-sync between analysis and render videos.
- Optional transparent-background graph output.
- More robust handling for 30/120+ fps.
- Cleaner argument validation and messages (ongoing).
