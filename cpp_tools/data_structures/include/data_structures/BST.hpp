#pragma once

#include "Node.hpp"
#include <concepts>
#include <cstdio>
#include <iostream>
#include <optional>
#include <queue>

namespace data_structures {

template <typename T>
  requires std::totally_ordered<T>
// doesn't allow duplicates
class BST {
private:
  Node<T> *root;

  // helper functions (recursive core logic)
private:
  // get smallest value starting at the current node
  Node<T> *findMin(Node<T> *node) const;
  // get largest value starting at the current node
  Node<T> *findMax(Node<T> *node) const;
  // Implemented with level-order traversal algorithm
  int height(Node<T> *node) const;
  Node<T> *insert(Node<T> *node, T value);
  /*
  3 cases for deletion:

          1. 🟢 Node is a leaf (no children)
          Example:
          5
          /
          3   ← delete this
          What you do:
          just remove it
          set parent's pointer to nullptr
          Why it’s easy:

          No structure needs fixing.


          2. 🟡 Node has ONE child

          Example:

              5
              /
              3
              \
              4   ← delete 3
          What you do:
          replace node with its child

          So:
              5
              /
              4
          Key idea:
          “Skip over the node”
          You’re basically reconnecting the parent directly to the child.


          CASE 3: Node has TWO children

          Example:

              10
              /  \
              5    15
                  /
              12

          Delete(10)

          Step 1:
          Find inorder successor
          -> smallest value in right subtree

          Right subtree:

              15
              /
          12

          Successor = 12

          Step 2:
          Copy successor value into current node

              12
              /  \
              5    15
                  /
              12

          Step 3:
          Delete the duplicate successor node from right subtree

              12
              /  \
              5    15

          Tree remains a valid BST.

          ------------------------------------------------

          Conceptual code flow:

          Node* successor = findMin(node->right);

          node->value = successor->value;

          node->right = remove(node->right, successor->value);

          return node;
  */
  Node<T> *remove(Node<T> *node, T value);
  bool contains(Node<T> *node, T value) const;

  void inorder(Node<T> *node) const;
  void preorder(Node<T> *node) const;
  void postorder(Node<T> *node) const;

  void destroy(Node<T> *node);

  // core API
public:
  explicit BST();
  explicit BST(T value);
  ~BST();

  // doesn't allow duplicates
  void insert(T value);
  void remove(T value);
  bool contains(T value) const;

  int height() const;

  std::optional<T> min() const;
  std::optional<T> max() const;

  void printInOrder() const;
  void printPreOrder() const;
  void printPostOrder() const;

  bool empty() const;
};

} // namespace data_structures
