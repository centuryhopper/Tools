#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <span>
#include <stdexcept>
#include <vector>

#include "../include/data_structures/BST.hpp"
#include "../include/data_structures/FenwickTree.hpp"

template <typename T> using BST = data_structures::BST<T>;

template <typename T = long long>
using FenwickTree = data_structures::FenwickTree<T>;

void FenwickTreeTests() {
  // =========================================================
  // Fenwick tree tests (1-based indices)
  // =========================================================

  // a[1..8] = 3 1 4 1 5 9 2 6
  // prefix  = 3 4 8 9 14 23 25 31
  const std::vector<long long> example{3, 1, 4, 1, 5, 9, 2, 6};

  // Returns true if fn throws std::out_of_range
  auto throwsOutOfRange = [](auto &&fn) {
    try {
      fn();
    } catch (const std::out_of_range &) {
      return true;
    }
    return false;
  };

  // -------------------------
  // Size constructor starts at zero
  // -------------------------
  {
    FenwickTree<> ft(8);

    assert(ft.size() == 8);

    for (std::size_t i = 1; i <= 8; i++) {
      assert(ft.get(i) == 0);
      assert(ft.prefix_sum(i) == 0);
    }
  }

  // -------------------------
  // Empty tree
  // -------------------------
  {
    FenwickTree<> ft(0);

    assert(ft.size() == 0);
    assert(ft.prefix_sum(0) == 0);
    assert(throwsOutOfRange([&] { (void)ft.get(1); }));
    assert(throwsOutOfRange([&] { ft.add(1, 5); }));

    std::vector<long long> none;
    FenwickTree<> fromEmptySpan{std::span<const long long>(none)};

    assert(fromEmptySpan.size() == 0);
    assert(fromEmptySpan.prefix_sum(0) == 0);
  }

  // -------------------------
  // Single element
  // -------------------------
  {
    std::vector<long long> v{42};
    FenwickTree<> ft{std::span<const long long>(v)};

    assert(ft.size() == 1);
    assert(ft.get(1) == 42);
    assert(ft.range_sum(1, 1) == 42);

    ft.add(1, -2);
    assert(ft.get(1) == 40);

    ft.set(1, 7);
    assert(ft.get(1) == 7);
  }

  // -------------------------
  // Span constructor: every element and every prefix
  // -------------------------
  {
    FenwickTree<> ft{std::span<const long long>(example)};

    assert(ft.size() == 8);

    // Even indices matter most: their slots hold multi-element chunk sums
    for (std::size_t i = 1; i <= 8; i++) {
      assert(ft.get(i) == example[i - 1]);
    }

    const long long expected[] = {0, 3, 4, 8, 9, 14, 23, 25, 31};

    for (std::size_t i = 0; i <= 8; i++) {
      assert(ft.prefix_sum(i) == expected[i]);
    }
  }

  // -------------------------
  // Span constructor: sizes that aren't powers of two
  // -------------------------
  for (std::size_t n : {2u, 3u, 5u, 6u, 7u, 9u, 13u, 31u, 33u}) {
    std::vector<long long> v(n);

    for (std::size_t k = 0; k < n; k++) {
      v[k] = static_cast<long long>(k * 3 + 1);
    }

    FenwickTree<> ft{std::span<const long long>(v)};

    assert(ft.size() == n);

    long long running = 0;

    for (std::size_t i = 1; i <= n; i++) {
      running += v[i - 1];
      assert(ft.prefix_sum(i) == running);
      assert(ft.get(i) == v[i - 1]);
    }
  }

  // -------------------------
  // Building from a span == building empty and calling add
  // -------------------------
  {
    FenwickTree<> fromSpan{std::span<const long long>(example)};
    FenwickTree<> fromAdds(example.size());

    for (std::size_t i = 1; i <= example.size(); i++) {
      fromAdds.add(i, example[i - 1]);
    }

    for (std::size_t i = 0; i <= example.size(); i++) {
      assert(fromSpan.prefix_sum(i) == fromAdds.prefix_sum(i));
    }
  }

  // -------------------------
  // range_sum known values
  // -------------------------
  {
    FenwickTree<> ft{std::span<const long long>(example)};

    assert(ft.range_sum(1, 8) == 31); // whole array
    assert(ft.range_sum(1, 1) == 3);  // first element (uses prefix_sum(0))
    assert(ft.range_sum(8, 8) == 6);  // last element
    assert(ft.range_sum(3, 6) == 19); // 4+1+5+9
    assert(ft.range_sum(2, 7) == 22); // 1+4+1+5+9+2
    assert(ft.range_sum(5, 5) == 5);
    assert(ft.range_sum(1, 4) == 9); // exactly one chunk (t[4])
    assert(ft.range_sum(5, 8) == 22);

    // Every (l, r) pair against a plain loop
    for (std::size_t l = 1; l <= 8; l++) {
      for (std::size_t r = l; r <= 8; r++) {
        long long expected = 0;

        for (std::size_t k = l; k <= r; k++) {
          expected += example[k - 1];
        }

        assert(ft.range_sum(l, r) == expected);
      }
    }
  }

  // -------------------------
  // add
  // -------------------------
  {
    FenwickTree<> ft{std::span<const long long>(example)};

    ft.add(3, 10); // a[3]: 4 -> 14

    assert(ft.get(3) == 14);
    assert(ft.prefix_sum(2) == 4); // before index 3: unchanged
    assert(ft.prefix_sum(3) == 18);
    assert(ft.prefix_sum(4) == 19);
    assert(ft.prefix_sum(8) == 41);
    assert(ft.range_sum(4, 8) == 23); // after index 3: unchanged
    assert(ft.get(2) == 1);           // neighbours untouched
    assert(ft.get(4) == 1);

    ft.add(6, -9); // a[6]: 9 -> 0
    assert(ft.get(6) == 0);
    assert(ft.prefix_sum(8) == 32);
  }

  // add on first and last index, and repeated adds
  {
    FenwickTree<> ft(8);

    ft.add(1, 5);
    ft.add(8, 7);

    assert(ft.get(1) == 5);
    assert(ft.get(8) == 7);
    assert(ft.prefix_sum(7) == 5);
    assert(ft.prefix_sum(8) == 12);

    ft.add(2, 3);
    ft.add(2, 4);
    ft.add(2, -1);
    assert(ft.get(2) == 6);
  }

  // -------------------------
  // set
  // -------------------------
  {
    FenwickTree<> ft{std::span<const long long>(example)};

    ft.add(3, 10); // a[3] = 14
    ft.set(3, 7);  // a[3] = 7

    assert(ft.get(3) == 7);
    assert(ft.prefix_sum(4) == 12);
    assert(ft.prefix_sum(8) == 34);

    // Setting to the current value changes nothing
    ft.set(4, 1);
    assert(ft.get(4) == 1);
    assert(ft.prefix_sum(8) == 34);

    // First and last index
    ft.set(1, 0);
    ft.set(8, 0);
    assert(ft.get(1) == 0);
    assert(ft.get(8) == 0);
    assert(ft.prefix_sum(8) == 25);

    // Twice in a row keeps the last value
    ft.set(2, 100);
    ft.set(2, 5);
    assert(ft.get(2) == 5);

    // Negative
    ft.set(5, -50);
    assert(ft.get(5) == -50);
  }

  // -------------------------
  // Negative values
  // -------------------------
  {
    std::vector<long long> v{-5, 3, -2, 0, -1, 4};
    FenwickTree<> ft{std::span<const long long>(v)};

    assert(ft.prefix_sum(6) == -1);
    assert(ft.range_sum(2, 4) == 1);
    assert(ft.get(1) == -5);
    assert(ft.get(4) == 0);
  }

  // -------------------------
  // Large values don't overflow long long
  // -------------------------
  {
    std::vector<long long> v(8, 2'000'000'000LL); // each > INT_MAX / 2
    FenwickTree<> ft{std::span<const long long>(v)};

    assert(ft.prefix_sum(8) == 16'000'000'000LL);
  }

  // -------------------------
  // Other value types
  // -------------------------
  {
    std::vector<int> v{1, 2, 3, 4, 5};
    FenwickTree<int> ft{std::span<const int>(v)};

    assert(ft.prefix_sum(5) == 15);
    ft.add(2, 10);
    assert(ft.range_sum(2, 3) == 15);
    ft.set(5, 0);
    assert(ft.prefix_sum(5) == 20);
  }
  {
    // Powers of two are exact in floating point, so == is safe here
    std::vector<double> v{0.5, 0.25, 1.5, 2.0};
    FenwickTree<double> ft{std::span<const double>(v)};

    assert(ft.prefix_sum(4) == 4.25);
    assert(ft.get(2) == 0.25);
    ft.add(3, 0.5);
    assert(ft.range_sum(3, 4) == 4.0);
  }

  // -------------------------
  // Out-of-range indices throw
  // -------------------------
  {
    FenwickTree<> ft{std::span<const long long>(example)}; // size 8

    assert(throwsOutOfRange([&] { ft.add(0, 1); }));
    assert(throwsOutOfRange([&] { ft.add(9, 1); }));
    assert(throwsOutOfRange([&] { (void)ft.get(0); }));
    assert(throwsOutOfRange([&] { (void)ft.get(9); }));
    assert(throwsOutOfRange([&] { ft.set(0, 1); }));
    assert(throwsOutOfRange([&] { ft.set(9, 1); }));
    assert(throwsOutOfRange([&] { (void)ft.prefix_sum(9); }));
    assert(throwsOutOfRange([&] { (void)ft.range_sum(0, 3); }));
    assert(throwsOutOfRange([&] { (void)ft.range_sum(3, 9); }));

    // A call that throws must leave the tree unchanged
    assert(ft.prefix_sum(8) == 31);
  }

  // -------------------------
  // Randomized: add/set/get/prefix_sum/range_sum vs a plain vector
  // -------------------------
  {
    std::mt19937_64 rng(12345); // fixed seed so failures are reproducible
    std::uniform_int_distribution<long long> val(-100, 100);

    for (std::size_t n = 1; n <= 40; n++) {
      std::vector<long long> a(n); // 0-based mirror: a[i - 1] is element i

      for (auto &x : a) {
        x = val(rng);
      }

      FenwickTree<> ft{std::span<const long long>(a)};
      std::uniform_int_distribution<std::size_t> idx(1, n);

      for (int op = 0; op < 60; op++) {
        const std::size_t i = idx(rng);

        if (rng() % 2) {
          const long long d = val(rng);
          ft.add(i, d);
          a[i - 1] += d;
        } else {
          const long long v = val(rng);
          ft.set(i, v);
          a[i - 1] = v;
        }
      }

      long long running = 0;
      assert(ft.prefix_sum(0) == 0);

      for (std::size_t i = 1; i <= n; i++) {
        running += a[i - 1];
        assert(ft.get(i) == a[i - 1]);
        assert(ft.prefix_sum(i) == running);
      }

      for (std::size_t l = 1; l <= n; l++) {
        long long expected = 0;

        for (std::size_t r = l; r <= n; r++) {
          expected += a[r - 1];
          assert(ft.range_sum(l, r) == expected);
        }
      }
    }
  }

  // -------------------------
  // lower_bound: smallest index whose prefix sum >= target
  // Not found = size() + 1. Change these asserts if you pick another
  // convention.
  // -------------------------
  {
    // a = 2 0 3 1 4, prefix sums = 2 2 5 6 10
    std::vector<long long> v{2, 0, 3, 1, 4};
    FenwickTree<> ft{std::span<const long long>(v)};

    assert(ft.lower_bound(0) == 1);
    assert(ft.lower_bound(1) == 1);
    assert(ft.lower_bound(2) == 1); // exact match, duplicated: smallest
    assert(ft.lower_bound(3) == 3); // not present: first one past it
    assert(ft.lower_bound(4) == 3);
    assert(ft.lower_bound(5) == 3); // exact match
    assert(ft.lower_bound(6) == 4);
    assert(ft.lower_bound(10) == 5);             // exactly the total
    assert(ft.lower_bound(11) == ft.size() + 1); // not found
  }

  // All ones: lower_bound(t) == t
  {
    FenwickTree<> ft(8);

    for (std::size_t i = 1; i <= 8; i++) {
      ft.add(i, 1);
    }

    for (long long t = 1; t <= 8; t++) {
      assert(ft.lower_bound(t) == static_cast<std::size_t>(t));
    }

    assert(ft.lower_bound(9) == ft.size() + 1);
  }

  // All zeros and empty
  {
    FenwickTree<> zeros(5);

    assert(zeros.lower_bound(0) == 1);
    assert(zeros.lower_bound(1) == zeros.size() + 1);

    FenwickTree<> empty(0);

    assert(empty.lower_bound(1) == empty.size() + 1);
  }

  // After updates
  {
    FenwickTree<> ft(6); // prefix: 0 0 0 0 0 0

    ft.add(4, 5); // prefix: 0 0 0 5 5 5
    assert(ft.lower_bound(1) == 4);

    ft.set(2, 3); // prefix: 0 3 3 8 8 8
    assert(ft.lower_bound(1) == 2);
    assert(ft.lower_bound(4) == 4);
    assert(ft.lower_bound(9) == ft.size() + 1);
  }

  // Randomized lower_bound vs a linear scan (non-negative values only)
  {
    std::mt19937_64 rng(777);
    std::uniform_int_distribution<long long> val(0, 10);

    for (std::size_t n = 1; n <= 40; n++) {
      std::vector<long long> a(n);

      for (auto &x : a) {
        x = val(rng);
      }

      FenwickTree<> ft{std::span<const long long>(a)};

      long long total = 0;

      for (long long x : a) {
        total += x;
      }

      for (long long t = 0; t <= total + 2; t++) {
        std::size_t expected = n + 1; // not found
        long long running = 0;

        for (std::size_t k = 1; k <= n; k++) {
          running += a[k - 1];

          if (running >= t) {
            expected = k;
            break;
          }
        }

        assert(ft.lower_bound(t) == expected);
      }
    }
  }
}

void BST_Test() {
  data_structures::BST<int> tree;

  // -------------------------
  // Empty tree tests
  // -------------------------

  assert(tree.empty() == true);
  assert(tree.contains(5) == false);

  // -------------------------
  // Insert tests
  // -------------------------

  tree.insert(10);
  tree.insert(5);
  tree.insert(15);
  tree.insert(3);
  tree.insert(7);
  tree.insert(12);
  tree.insert(20);

  /*
                              10
                            /     \
                           5       15
                          / \     /  \
                         3   7   12  20

  */

  assert(tree.empty() == false);

  // -------------------------
  // Contains tests
  // -------------------------

  assert(tree.contains(10) == true);
  assert(tree.contains(5) == true);
  assert(tree.contains(20) == true);

  assert(tree.contains(999) == false);
  assert(tree.contains(-1) == false);

  // -------------------------
  // Min / Max tests
  // -------------------------

  assert(tree.min() == 3);
  assert(tree.max() == 20);

  // -------------------------
  // Duplicate insert tests
  // -------------------------

  tree.insert(10);
  tree.insert(5);

  // Behavior depends on your implementation:
  // - ignore duplicates
  // - count duplicates
  // - allow duplicates on one side
  //
  // Add assertions once you decide behavior.

  // -------------------------
  // Remove leaf node
  // -------------------------

  // tree.printInOrder();

  tree.remove(3);

  assert(tree.contains(3) == false);

  // tree.printInOrder();

  // -------------------------
  // Remove node with one child
  // -------------------------

  tree.remove(5);

  assert(tree.contains(5) == false);

  // -------------------------
  // Remove node with two children
  // -------------------------

  tree.remove(10);

  assert(tree.contains(10) == false);

  // Remaining nodes should still exist
  assert(tree.contains(7) == true);
  assert(tree.contains(12) == true);
  assert(tree.contains(15) == true);

  // -------------------------
  // Remove non-existent value
  // -------------------------

  tree.remove(999);

  // Should not crash

  // -------------------------
  // Stress-ish insertion test
  // -------------------------

  BST<int> bigTree;

  for (int i = 0; i < 1000; i++) {
    bigTree.insert(i);
  }

  for (int i = 0; i < 1000; i++) {
    assert(bigTree.contains(i) == true);
  }

  assert(bigTree.contains(5000) == false);

  // -------------------------
  // Traversal tests
  // -------------------------

  BST<int> traversalTree;

  traversalTree.insert(8);
  traversalTree.insert(3);
  traversalTree.insert(10);
  traversalTree.insert(1);
  traversalTree.insert(6);
  traversalTree.insert(14);

  std::cout << "Inorder traversal should be sorted:\n";
  traversalTree.printInOrder();

  // Empty tree
  {
    BST<int> emptyTree;

    assert(emptyTree.height() == -1);
  }

  // Single node
  {
    BST<int> single;

    single.insert(50);

    assert(single.height() == 0);
  }

  // Perfect-ish balanced tree
  {
    BST<int> tree;

    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);
    tree.insert(60);
    tree.insert(80);

    /*
            50
        /    \
        30      70
    / \     / \
    20  40   60 80
    */

    assert(tree.height() == 2);
  }

  // Left-skewed tree
  {
    BST<int> leftSkewed;

    leftSkewed.insert(50);
    leftSkewed.insert(40);
    leftSkewed.insert(30);
    leftSkewed.insert(20);

    /*
            50
        /
        40
        /
    30
    /
    20
    */

    assert(leftSkewed.height() == 3);
  }

  // Right-skewed tree
  {
    BST<int> rightSkewed;

    rightSkewed.insert(10);
    rightSkewed.insert(20);
    rightSkewed.insert(30);
    rightSkewed.insert(40);

    /*
        10
        \
        20
            \
            30
                \
                40
    */

    assert(rightSkewed.height() == 3);
  }

  // Uneven tree
  {
    BST<int> uneven;

    uneven.insert(50);
    uneven.insert(30);
    uneven.insert(70);
    uneven.insert(20);
    uneven.insert(10);
    uneven.insert(5);

    /*
            50
            /  \
        30    70
        /
        20
        /
    10
    /
    5
    */

    assert(uneven.height() == 4);
  }
}

int main() {

  FenwickTreeTests();

  std::cout << "\nAll tests passed!\n";

  return 0;
}