
#include <string>
#include <chrono>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/chrono.h>

class ScopedTimer {
public:
    explicit ScopedTimer(std::string label)
        : label_(std::move(label)), start_(std::chrono::high_resolution_clock::now()) {}

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
        fmt::print("{}: {} ms\n", label_, duration.count());
    }

private:
    std::string label_;
    std::chrono::high_resolution_clock::time_point start_;
};