#include "FFmpegPipe.h"
#include "v_ArgumentParser.h"


FFmpegPipe::FFmpegPipe(int width, int height, int fps, const std::string& outPath){
    
    std::string cmd = std::format(
        "ffmpeg -y -loglevel error -f rawvideo -vcodec rawvideo -pix_fmt bgr24 -s {}x{} -r {} -i - "
        "-c:v libx264 -preset fast -crf 20 -pix_fmt yuv420p \"{}\"", 
        width, height, fps, outPath
    );
    std::println("Opening FFmpeg Pipe: {}", cmd);

#ifdef _WIN32
    pipe = _popen(cmd.c_str(), "wb");
#else
    pipe = popen(cmd.c_str(), "w");
#endif

    if (!pipe) {
        throw std::runtime_error("Failed to open FFmpeg pipe! Is ffmpeg in your PATH?");
    }
}

FFmpegPipe::~FFmpegPipe() {
    if (pipe) {
        std::println("Closing FFmpeg Pipe...");
#ifdef _WIN32
        _pclose(pipe);
#else
        pclose(pipe);
#endif
    }
}

void FFmpegPipe::write(const cv::Mat& frame) {
    if (!pipe) return;
    fwrite(frame.data, 1, frame.total() * frame.elemSize(), pipe);
}