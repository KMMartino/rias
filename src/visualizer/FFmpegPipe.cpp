#include "FFmpegPipe.h"
#include <print>


FFmpegPipe::FFmpegPipe(int width, int height, int fps, const VisualizerConfig& config){
    std::string codecOpts;

    switch (config.encoder) {
        case EncoderType::NVENC:
            std::println("Encoder: NVIDIA NVENC (h264_nvenc)");
            codecOpts = "-c:v h264_nvenc -preset p4 -rc constqp -qp 21 -g 60 -bf 0";
            break;

        case EncoderType::AMF:
            std::println("Encoder: AMD AMF (h264_amf)");
            codecOpts = "-c:v h264_amf -usage transcoding -rc cqp -qp_i 20 -qp_p 22 -qp_b 24 -g 60";
            break;

        case EncoderType::CPU:
        default:
            std::println("Encoder: CPU (libx264)");
            codecOpts = "-c:v libx264 -preset fast -crf 19 -g 60 -bf 0 -keyint_min 60";
            break;
    }
    
    std::string cmd = std::format(
        "ffmpeg -y -loglevel error "
        "-f rawvideo -vcodec rawvideo -pix_fmt bgr24 -s {}x{} -r {} -i - "
        "-i \"{}\" "
        "-map 0:v -map 1:a? -c:a copy -shortest "
        "{} -pix_fmt yuv420p \"{}\"",
        width, height, fps, config.videoPath, codecOpts, config.outPath
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