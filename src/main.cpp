#include <opencv2/opencv.hpp>
#include <print>
#include "FrameProcessor.h"

int main() {
    std::println("Rias Project: Starting up...");
    cv::Mat img1 = cv::Mat::zeros(100, 100, CV_8UC3);
    cv::Mat img2 = img1.clone();
    cv::rectangle(img2, cv::Rect(10, 10, 5, 5), cv::Scalar(255, 255, 255), -1);

    FrameProcessor processor(30);

    bool result1 = processor.is_frame_unique(img1, img1);
    std::println("Test1 -identical- {}", result1 ? "Unique" : "Duplicate");

    bool result2 = processor.is_frame_unique(img1, img2);
    std::println("Test1 -unique- {}", result2 ? "Unique" : "Duplicate");
}