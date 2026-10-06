#include <cassert>
#include <chrono>
#include <print>

#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"

int main() {

  const fs::path DIR_PATH =
      fs::path("/home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/"
               "tests/");

  auto start = std::chrono::steady_clock::now();
  auto files = similar_files_checker::walkDirectoryRecursivelyParallel(
      DIR_PATH / "directory_contents/", 4);
  // auto files = similar_files_checker::walkDirectoryRecursively("");
  std::chrono::duration<double, std::milli> elapsed =
      std::chrono::steady_clock::now() - start;

  std::println("{} files in {}", files.size(), elapsed);

  auto result = similar_files_checker::findSameSizeFiles(files)
                    .and_then(similar_files_checker::findSamePartialHashFiles)
                    .and_then(similar_files_checker::findSameFullHashFiles);

  if (!result) {
    std::println(stderr, "error: {}", result.error());
    return 1;
  }

  similar_files_checker::handleDuplicates(*result, false);

  return 0;
}