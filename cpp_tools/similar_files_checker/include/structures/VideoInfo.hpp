
#include <filesystem>
#include <chrono>

struct VideoInfo {
    std::filesystem::path path;
    std::chrono::system_clock::time_point timestamp;

};