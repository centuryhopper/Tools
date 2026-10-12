#include "../include/data_structures/FenwickTree.hpp"

#include <cassert>
#include <chrono> // std::chrono, std::chrono_literals
#include <cstddef>
#include <filesystem>      // std::filesystem
#include <memory_resource> // std::pmr
#include <ranges>          // std::ranges, std::ranges::views
#include <stdexcept>
#include <string> // std::string_literals ("abc"s)

namespace fs = std::filesystem;
namespace rng = std::ranges;
namespace rv = std::views;
namespace chr = std::chrono;
namespace pmr = std::pmr;

using namespace std::literals;

namespace data_structures {

template <typename T>
[[nodiscard]] constexpr std::size_t
FenwickTree<T>::lsb(std::size_t i) noexcept {
  return i & -i;
}

template <typename T>
void FenwickTree<T>::check_index(std::size_t index) const {
  if (index < 1 || index > size()) {
    throw std::out_of_range("index is out of bounds");
  }
}

template <typename T>
FenwickTree<T>::FenwickTree(std::size_t n) : tree(n + 1, 0) {}

template <typename T> FenwickTree<T>::FenwickTree(std::span<const T> values) {
  tree.resize(values.size() + 1);
  for (const auto &[i, val] : rv::enumerate(values)) {
    this->tree[i + 1] = val;
  }

  for (size_t i = 1; i < this->tree.size(); i++) {
    size_t j = i + lsb(i);
    if (j <= values.size()) {
      // add parent value to its current jth index
      this->tree[j] += this->tree[i];
    }
  }
}

template <typename T>
[[nodiscard]] T FenwickTree<T>::prefix_sum(std::size_t index) const {
  if (index != 0) {
    check_index(index);
  }
  T sum = 0;
  // walk up the tree (towards 0)
  while (index > 0) {
    sum += this->tree[index];
    index -= lsb(index);
  }

  return sum;
}

template <typename T>
[[nodiscard]] T FenwickTree<T>::range_sum(std::size_t left,
                                          std::size_t right) const {
  check_index(left);
  check_index(right);
  assert(left <= right);
  return this->prefix_sum(right) - this->prefix_sum(left - 1);
}

template <typename T> void FenwickTree<T>::add(std::size_t index, T delta) {
  check_index(index);
  while (index < this->tree.size()) {
    this->tree[index] += delta;
    index += lsb(index);
  }
}

template <typename T>
[[nodiscard]] T FenwickTree<T>::get(std::size_t index) const {
  return range_sum(index, index);
}
template <typename T> void FenwickTree<T>::set(std::size_t index, T value) {
  check_index(index);
  const T cur = get(index);
  const T delta = value - cur;
  add(index, delta);
}

template <typename T>
[[nodiscard]] std::size_t FenwickTree<T>::size() const noexcept {
  // since index 0 doesnt count in the tree
  return this->tree.size() - 1;
}
template <typename T>
[[nodiscard]] std::size_t FenwickTree<T>::lower_bound(T target) const {

  /*
[1,2,3,4,5,6,7]


[2, 0, 3, 1, 4]
  */
  size_t lo = 1;
  size_t hi = tree.size();
  size_t mid = 0;
  while (lo < hi) {
    mid = lo + ((hi - lo) / 2);
    T midSum = prefix_sum(mid);
    // too small
    if (midSum < target) {
      // no way lo is at target so we exclude it
      lo = mid + 1;
    } else if (midSum >= target) {
      // mid could still be the target since that answer is AT LEAST as big as
      // target
      hi = mid;
    }
  }
  return lo;

  // size_t n = tree.size();
  // size_t retVal = n - 1;
  // for (size_t i = n - 1; i > 0; i--) {
  //   T preSum = prefix_sum(i);
  //   if (preSum >= target) {
  //     retVal = i;
  //   }
  // }

  // return retVal;
}

template class FenwickTree<long long>;
template class FenwickTree<int>;
template class FenwickTree<double>;

} // namespace data_structures
