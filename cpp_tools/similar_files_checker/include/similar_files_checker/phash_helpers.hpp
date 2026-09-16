#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/img_hash.hpp>


namespace fs = std::filesystem;

cv::Mat computePHash(const fs::path& path)
{
    cv::Mat image = cv::imread(path.string());

    if (image.empty())
    {
        // throw std::runtime_error("Failed to read image: " + path.string());
        fmt::print(stderr, "Skipping unreadable image: {}\n", path.string());
        return {};
    }

    static cv::Ptr<cv::img_hash::PHash> hasher =
        cv::img_hash::PHash::create();

    cv::Mat hash;
    hasher->compute(image, hash);

    return hash;
}

double comparePHash(const cv::Mat& hashA, const cv::Mat& hashB)
{
    static cv::Ptr<cv::img_hash::PHash> hasher =
        cv::img_hash::PHash::create();

    return hasher->compare(hashA, hashB);
}