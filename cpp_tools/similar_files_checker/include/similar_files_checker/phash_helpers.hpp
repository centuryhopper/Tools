#include "fmt/core.h"
#include "fmt/ranges.h"
#include <expected>
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



// A struct is a simple bundle of related data. This one holds everything we remember about one video after extracting its frames.
struct VideoHashCache {
    std::string path;              // the file path, so we know which video this is
    std::vector<cv::Mat> hashes;   // the sampled frames, stored as images in memory
};


// Hash a frame that's already in memory.
cv::Mat computeFramePHash(const cv::Mat& frame)
{
    thread_local auto hasher = cv::img_hash::PHash::create();
    cv::Mat hash;
    hasher->compute(frame, hash);
    return hash;   // an 8-byte (64-bit) hash
}

// A function that takes a video path, how many frames to sample, and what size
// to shrink them to, and returns a filled-in VideoFrameCache.
// "const std::string&" means: pass the string by reference (no copy) and
// promise not to modify it. This is the standard way to pass strings in C++.
std::expected<VideoHashCache, std::string> extractFrameHashes(const std::string& path, int numSamples) {
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

        if (totalFrames <= 0) {       // no frames (or the file doesn't report a count)
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
            cv::Mat frame;
            if (!cap.read(frame) || frame.empty())
                break;   // end of video

            bool isSampleFrame = (frameNumber % step == 0);
            ++frameNumber;

            if (!isSampleFrame)
                continue;

            cache.hashes[slot] = computeFramePHash(frame);

            #pragma omp critical
            fmt::print("{} slot {:2}: {}\n", fs::path(path).filename().string(), slot,
                    fmt::join(cv::Mat_<uchar>(cache.hashes[slot]), ""));

            ++slot;
        }

        return cache;   // hand the filled-in cache back to the caller
        // cap is automatically closed here when it goes out of scope — C++ cleans up objects at the end of the block they were created in.
    }
    catch (const cv::Exception& e)
    {
        return std::unexpected(std::string("OpenCV error: ") + e.what());
    }
}

struct VideoSimilarity {
    double matchRatio;   // fraction of comparable frames that matched
    int comparable;      // how many slots both videos had
};

// Both are passed as "const &" — read-only references, no copying of the frames.
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