#include <cassert>
#include <chrono>
#include <print>

#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"
#include "../include/similar_files_checker/scoped_timer.hpp"

int main() {
  // Force line buffering in code to get real-time output
  std::setvbuf(stdout, nullptr, _IOLBF, 0); // from <cstdio>

  // /home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/tests/directory_contents/
  const fs::path DIR_PATH = fs::path("/home/leo_zhang/synology/root/");

  auto start = std::chrono::steady_clock::now();
  std::println("walking the directory in parallel...");
  const std::vector<fs::path> files = [&]() {
    ScopedTimer timer("walkDirectoryRecursivelyParallel execution time");
    return similar_files_checker::walkDirectoryRecursivelyParallel(DIR_PATH,
                                                                   16);
  }();

  // auto files = similar_files_checker::walkDirectoryRecursively("");
  std::chrono::duration<double, std::milli> elapsed =
      std::chrono::steady_clock::now() - start;

  std::println("{} files in {}", files.size(), elapsed);

  auto result =
      similar_files_checker::findSameSizeFiles(files)
          .and_then([](const std::vector<fs::path> &paths) {
            ScopedTimer timer("findSamePartialHashFiles execution time");
            return similar_files_checker::findSamePartialHashFiles(paths);
          })
          .and_then([](const std::vector<fs::path> &paths) {
            ScopedTimer timer("findSameFullHashFiles execution time");
            return similar_files_checker::findSameFullHashFiles(paths);
          });

  return result
      .transform([](auto &dupes) {
        similar_files_checker::handleDuplicates(dupes, false);
        return 0; // success → exit code 0
      })
      .transform_error([](const auto &err) {
        std::println(stderr, "error: {}", err);
        return err; // log, keep the error
      })
      .value_or(1); // error → exit code 1;
}
