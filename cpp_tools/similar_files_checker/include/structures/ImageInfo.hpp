#pragma once

#include <filesystem>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/img_hash.hpp>
#include "MediaInfo.hpp"



struct ImageInfo : MediaInfo {   // public inheritance by default
    cv::Mat phash;
};
