#pragma once

#include <filesystem>
#include <chrono>

struct MediaInfo {
    std::filesystem::path path;
    std::chrono::system_clock::time_point timestamp;
};



