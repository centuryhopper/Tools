


#include <expected>
#include <filesystem>
#include <opencv2/img_hash/phash.hpp>
#include <opencv2/opencv.hpp>
#include <print>
#include "../include/structures/VideoHashCache.hpp"
#include "../include/structures/VideoSimilarity.hpp"
#include "fmt/core.h"
#include "fmt/ranges.h"


cv::Mat computePHash(const std::filesystem::path& path)
{
    cv::Mat image = cv::imread(path.string());

    if (image.empty())
    {
        // throw std::runtime_error("Failed to read image: " + path.string());
        std::println(stderr, "Skipping unreadable image: {}", path.string());
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

cv::Mat computeFramePHash(const cv::Mat& frame)
{
    thread_local auto hasher = cv::img_hash::PHash::create();
    cv::Mat hash;
    hasher->compute(frame, hash);
    return hash;   // an 8-byte (64-bit) hash
}


std::expected<VideoHashCache, std::string> extractFrameHashes(const std::string& path, int numSamples)
{
    try
    {
        VideoHashCache cache;   // create an empty cache object
        cache.path = path;       // remember which file it came from

        cv::VideoCapture cap(path);   // open the video file for reading

        if (!cap.isOpened()) {        // if opening failed (bad path, unsupported codec...)
            // Throwing stops this function immediately and sends the error to
            // whoever called it (they can catch it with try/catch).
            return std::unexpected("Could not open video: " + path);
        }

        // Ask the video how many frames it has. cap.get() returns a double,
        // so static_cast<int> converts it to an integer explicitly.
        int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));

        // no frames (or the file doesn't report a count)
        if (totalFrames <= 0) {
            return std::unexpected("Empty or unreadable video: " + path);
        }

        // Don't try to sample more frames than the video actually has.
        int samples = std::min(numSamples, totalFrames);

        // Pre-allocate room in the vector for 'samples' items. This is just a
        // performance hint so it doesn't have to grow repeatedly while we add frames.
        cache.hashes.resize(samples);

        // step is the spacing. totalFrames / samples asks: if I want 12 things spread across 70, how far apart should they be? Answer: about every 5th. The % step == 0 test is "is this frame a multiple of 5?" — true for frames 0, 5, 10, 15, and so on.
        int step = std::max(1, totalFrames / samples);
        int slot = 0;
        int frameNumber = 0;

        while (slot < samples)
        {
            if (!cap.grab())
                break;   // end of video

            cv::Mat frame;
        
            bool isSampleFrame = (frameNumber % step == 0);
            ++frameNumber;

            if (!isSampleFrame)
                continue;

            if (!cap.retrieve(frame) || frame.empty())
                break;

            cache.hashes[slot] = computeFramePHash(frame);

            //#pragma omp critical
            // fmt::print("{} slot {:2}: {}\n", std::filesystem::path(path).filename().string(), slot, fmt::join(cv::Mat_<uchar>(cache.hashes[slot]), ""));

            ++slot;
        }

        cache.hashes.resize(slot);   // drop unfilled slots if the video ended early

        return cache;   // hand the filled-in cache back to the caller
        // cap is automatically closed here when it goes out of scope — C++ cleans up objects at the end of the block they were created in.
    }
    catch (const cv::Exception& e)
    {
        return std::unexpected(std::string("OpenCV error: ") + e.what());
    }
}

VideoSimilarity compareCachedHashes(const VideoHashCache& a, const VideoHashCache& b) {
    const int TOLERABLE_DISTANCE = 8;   // Hamming distance on the 64-bit hash

    int n = static_cast<int>(std::min(a.hashes.size(), b.hashes.size()));
    int comparable = 0;
    int matched = 0;

    for (int i = 0; i < n; ++i)
    {
        if (a.hashes[i].empty() || b.hashes[i].empty())
            continue;   // one side failed to read this slot

        comparable++;
        auto d = comparePHash(a.hashes[i], b.hashes[i]);
        // #pragma omp critical
        // fmt::print("  slot {:2}: distance {}\n", i, d);
        
        if (d <= TOLERABLE_DISTANCE)
          matched++;
    }

    if (comparable < 5)
        return {0.0, comparable};   // too little data to trust

    return {static_cast<double>(matched) / comparable, comparable};
}

