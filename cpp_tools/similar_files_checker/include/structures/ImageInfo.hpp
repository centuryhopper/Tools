#pragma once

#include "MediaInfo.hpp"
#include <chrono>
#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/img_hash.hpp>

struct ImageInfo : MediaInfo
{ // public inheritance by default
    cv::Mat phash;
};
