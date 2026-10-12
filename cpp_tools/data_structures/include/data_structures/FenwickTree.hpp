#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace data_structures {

template <typename T = long long> class FenwickTree {
public:
  // Construction
  explicit FenwickTree(std::size_t n);             // n elements, all zero
  explicit FenwickTree(std::span<const T> values); // O(n) build from raw values

  // Core operations (public indices are 0-based)
  void add(std::size_t index, T delta);
  [[nodiscard]] T prefix_sum(std::size_t index) const; // sum of [0, index]
  [[nodiscard]] T range_sum(std::size_t left,
                            std::size_t right) const; // sum of [left, right]

  // Convenience
  [[nodiscard]] T get(std::size_t index) const;
  void set(std::size_t index, T value);
  [[nodiscard]] std::size_t size() const noexcept;

  // Stretch goal: smallest index whose prefix sum >= target (requires
  // non-negative values)
  [[nodiscard]] std::size_t lower_bound(T target) const;

private:
  // returns least significant bit of i
  [[nodiscard]] static constexpr std::size_t lsb(std::size_t i) noexcept;
  void check_index(std::size_t index) const; // throws or asserts on a bad index

  std::vector<T> tree; // size n + 1, slot 0 unused
};


} // namespace data_structures
