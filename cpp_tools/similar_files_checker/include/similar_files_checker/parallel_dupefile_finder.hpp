#pragma once

#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace similar_files_checker {

// iterative approach to recursively walk through a directory and collect all file paths (stack data structure implementation)
std::vector<fs::path> walkDirectoryRecursively(const fs::path& path);

} // namespace similar_files_checker