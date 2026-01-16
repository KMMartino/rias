#include <opencv2/opencv.hpp>
#include <print>
#include <vector>

int main() {
    // 1. Test C++23 std::println
    std::println("Rias Project: Starting up...");

    // 2. Test OpenCV Linking
    // Just creating a dummy matrix to ensure the linker finds the libs
    cv::Mat image = cv::Mat::zeros(100, 100, CV_8UC3);
    
    std::println("OpenCV Linked successfully. Matrix size: {} x {}", image.rows, image.cols);
    std::println("OpenCV Version: {}", CV_VERSION);

    return 0;
}