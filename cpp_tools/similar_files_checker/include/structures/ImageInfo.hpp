#include <filesystem>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/img_hash.hpp>


struct ImageInfo {
    std::filesystem::path path;
    std::chrono::system_clock::time_point timestamp;
    cv::Mat phash;
};
