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
#include <blake3.h>

#define XXH_INLINE_ALL
#include <xxhash.h>   // sudo apt install libxxhash-dev

namespace fs = std::filesystem;

using Digest = std::array<uint8_t, BLAKE3_OUT_LEN>;  // 32 bytes

namespace similar_files_checker {

    struct FileError {
        fs::path path;
        std::error_code ec;
    };

    struct DigestHash {
        size_t operator()(const Digest& d) const noexcept {
            size_t h;
            memcpy(&h, d.data(), sizeof h);
            return h;
        }
    };

// iterative approach to recursively walk through a directory and collect all file paths (stack data structure implementation)
std::vector<fs::path> walkDirectoryRecursively(const fs::path& path);

std::vector<fs::path> walkDirectoryRecursivelyParallel(const fs::path& path, uint numThreads);

[[nodiscard]] std::expected<std::vector<fs::path>, std::string> findSameSizeFiles(const std::vector<fs::path>& files);

std::optional<std::uint64_t> partialHash(const fs::path& p, std::size_t n);

[[nodiscard]] std::expected<std::vector<fs::path>, std::string> findSamePartialHashFiles(const std::vector<fs::path>& files);

std::optional<Digest> fullHash(const fs::path& path);

[[nodiscard]] std::expected<std::unordered_map<Digest, std::vector<fs::path>, DigestHash>, std::string> findSameFullHashFiles(const std::vector<fs::path>& files);

}

