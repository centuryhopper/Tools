#include <cassert>
#include <print>
#include <chrono>

#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"


int main() {
    
    auto start = std::chrono::steady_clock::now();
    // /home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/tests/directory_contents/
    auto files = similar_files_checker::walkDirectoryRecursivelyParallel("/home/leo_zhang/projects/Tools/cpp_tools/similar_files_checker/tests/directory_contents/", 4);
    // auto files = similar_files_checker::walkDirectoryRecursively("");
    std::chrono::duration<double, std::milli> elapsed = std::chrono::steady_clock::now() - start;
    
    std::println("{} files in {}", files.size(), elapsed);

    auto filesFiltered1 = similar_files_checker::findSameSizeFiles(files)
        .and_then(similar_files_checker::findSamePartialHashFiles);

    if (filesFiltered1.has_value())
    {
        for (const auto& path : *filesFiltered1)
            std::println("{}", path.string());
    }


    return 0;
}