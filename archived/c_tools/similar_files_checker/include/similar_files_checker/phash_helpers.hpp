#include <expected>
#include <filesystem>
#include <opencv2/opencv.hpp>
#include <opencv2/img_hash.hpp>
#include "../structures/VideoHashCache.hpp"
#include "../structures/VideoSimilarity.hpp"


namespace fs = std::filesystem;

cv::Mat computePHash(const fs::path& path);

double comparePHash(const cv::Mat& hashA, const cv::Mat& hashB);

// Hash a frame that's already in memory.
cv::Mat computeFramePHash(const cv::Mat& frame);

// A function that takes a video path, how many frames to sample, and what size
// to shrink them to, and returns a filled-in VideoFrameCache.
// "const std::string&" means: pass the string by reference (no copy) and
// promise not to modify it. This is the standard way to pass strings in C++.
std::expected<VideoHashCache, std::string> extractFrameHashes(const std::string& path, int numSamples);

// Both are passed as "const &" — read-only references, no copying of the frames.
VideoSimilarity compareCachedHashes(const VideoHashCache& a, const VideoHashCache& b);