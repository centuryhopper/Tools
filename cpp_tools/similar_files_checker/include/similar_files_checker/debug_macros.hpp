#pragma once
#include <print>
#include <ranges>

#ifndef NDEBUG
#define PRINT_SIZE_GROUPS(groups)                                          \
    do {                                                                   \
        for (const auto& [size_, files_] : (groups)) {                     \
            std::println("size:{}, paths: {}", size_,                      \
                files_ | std::views::transform(                            \
                    [](const fs::path& p) { return p.string(); }));        \
        }                                                                  \
    } while (0)
#else
#define PRINT_SIZE_GROUPS(groups) do {} while (0)
#endif

namespace similar_files_checker {

// debug_macros

} // namespace similar_files_checker