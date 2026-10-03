#include <cassert>
#include <print>

#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"


int main() {
    // assert(1 + 1 == 2);

    // std::cout << "Tests passed!" << std::endl;
    auto test_results = similar_files_checker::walkDirectoryRecursively(TEST_DATA_DIR);std::println("test results size: {}", test_results.size());

    for (const auto& path : test_results)
        std::println("{}", path.string());

    return 0;
}