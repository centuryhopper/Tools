#include "../include/similar_files_checker/parallel_dupefile_finder.hpp"
#include <filesystem>
#include <print>
#include <algorithm>

namespace similar_files_checker {

} // namespace similar_files_checker

std::vector<fs::path> similar_files_checker::walkDirectoryRecursively(const fs::path &path)
{
    using namespace std;
    vector<fs::path> results, stack;
    stack.push_back(path);

    while (!stack.empty())
    {
        fs::path parentPath = stack.back();
        stack.pop_back();
        if (fs::is_directory(parentPath))
        {
            // std::println("{} is a directory", parentPath.string());
            // process children
            vector<fs::path> childDir;

            for (const auto& childPath : fs::directory_iterator(parentPath))
            {
                // std::println("child path: {}", childPath.string());
                childDir.push_back(childPath);
            }
            std::sort(childDir.begin(), childDir.end(), [](const fs::path& f1, const fs::path& f2) {
                return f1.string() > f2.string();
            });
            for (const auto& childPath : childDir)
            {
                stack.push_back(childPath);
            }
        }
        results.push_back(std::move(parentPath));
    }

    return results;
}