#pragma once

// A struct is a simple bundle of related data. This one holds everything we remember about one video after extracting its frames.
#include <opencv4/opencv2/core/mat.hpp>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

struct VideoHashCache {
    // the file path, so we know which video this is
    std::string path;
    // the sampled frames, stored as images in memory
    std::vector<cv::Mat> hashes;
};
