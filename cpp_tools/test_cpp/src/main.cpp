

#include <fmt/core.h>
#include <iostream>
#include "../include/bst/BST.hpp"

// cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

int main()
{
  std::cout << "Hello from test_cpp!" << std::endl;
  fmt::print("Hello {}\n", "world");
  return 0;
}
