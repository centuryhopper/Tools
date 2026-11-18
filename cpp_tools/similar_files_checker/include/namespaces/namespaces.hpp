#pragma once

#include <filesystem>
#include <ranges> // std::ranges, std::ranges::views

namespace fs = std::filesystem;
namespace chr = std::chrono;
;
namespace rng = std::ranges;
namespace rv = std::views;

using Timestamp = chr::system_clock::time_point;
