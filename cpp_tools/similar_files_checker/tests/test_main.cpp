#include <cassert>
#include <print>
#include <chrono>

#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"


int main() {
    
    auto start = std::chrono::steady_clock::now();
    // /home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/tests/directory_contents/
    auto files = similar_files_checker::walkDirectoryRecursivelyParallel("");
    // auto files = similar_files_checker::walkDirectoryRecursively("");
    std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - start;
    
    std::println("{} files in {}", files.size(), elapsed);
    
    // for (const auto& path : files)
    //     std::println("{}", path.string());

    return 0;
}