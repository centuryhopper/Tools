#pragma once

#include <expected>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <condition_variable>
#include <cstdint>
#include <execution>
#include <expected>
#include <optional>
#include <print>
#include <algorithm>
#include <system_error>
#include <thread>
#include <ranges>
#include <fstream>


#define XXH_INLINE_ALL
#include <xxhash.h>   // sudo apt install libxxhash-dev

namespace fs = std::filesystem;

namespace similar_files_checker {

    struct FileSizeError {
        fs::path path;
        std::error_code ec;
    };

// iterative approach to recursively walk through a directory and collect all file paths (stack data structure implementation)
std::vector<fs::path> walkDirectoryRecursively(const fs::path& path);

std::vector<fs::path> walkDirectoryRecursivelyParallel(const fs::path& path, uint numThreads);

[[nodiscard]] std::expected<std::vector<fs::path>, std::string> findSameSizeFiles(const std::vector<fs::path>& files);

std::optional<std::uint64_t> partialHash(const fs::path& p, std::size_t n);

[[nodiscard]] std::expected<std::vector<fs::path>, std::string> findSamePartialHashFiles(const std::vector<fs::path>& files);

}

