#include <cassert>
#include <chrono>
#include <print>

#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"

int main() {

  const fs::path DIR_PATH =
      fs::path("/home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/"
               "tests/");

  const fs::path OUTPUT_PATH =
      DIR_PATH / std::format("output/output_{:%Y-%m-%d_%H-%M-%S}.txt",
                             floor<std::chrono::seconds>(
                                 std::chrono::system_clock::now()));
  fs::create_directories(OUTPUT_PATH.parent_path());

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

  std::ofstream out(OUTPUT_PATH);
  for (const auto &[digestHash, paths] : *result) {
    for (const fs::path &path : paths)
      std::println(out, "{}", path.string());
    std::println(out, "");
  }

  return 0;
}