
#include <chrono>
#include <print>
#include <string>
#include <format>  // std::format
#include <ranges>
#include <algorithm>


namespace chr = std::chrono;
namespace rng = std::ranges;

class ScopedTimer {
public:
  explicit ScopedTimer(std::string label)
      : label(std::move(label)), start(chr::high_resolution_clock::now()) {}

  ~ScopedTimer() {
    auto end = chr::high_resolution_clock::now();
    auto duration = chr::duration_cast<chr::milliseconds>(end - start);
    auto now =
        chr::zoned_time{chr::current_zone(),
                        chr::floor<chr::seconds>(chr::system_clock::now())};
    auto nowFormat = std::format("[{:%Y/%m/%d_%H-%M-%S}]", now);
    std::println("{} {}: {} ms", nowFormat, label, duration.count());
  }

private:
  std::string label;
  chr::high_resolution_clock::time_point start;
};