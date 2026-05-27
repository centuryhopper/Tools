#include <cassert>
#include <iostream>

#include "../include/bst/BST.hpp"

int main()
{
    BST<int> tree;

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

    // tree.remove(3);

    // assert(tree.contains(3) == false);

    // -------------------------
    // Remove node with one child
    // -------------------------

    // tree.remove(5);

    // assert(tree.contains(5) == false);

    // -------------------------
    // Remove node with two children
    // -------------------------

    // tree.remove(10);

    // assert(tree.contains(10) == false);

    // Remaining nodes should still exist
    assert(tree.contains(7) == true);
    assert(tree.contains(12) == true);
    assert(tree.contains(15) == true);

    // -------------------------
    // Remove non-existent value
    // -------------------------

    // tree.remove(999);

    // Should not crash

    // -------------------------
    // Stress-ish insertion test
    // -------------------------

    BST<int> bigTree;

    for (int i = 0; i < 1000; i++)
    {
        bigTree.insert(i);
    }

    for (int i = 0; i < 1000; i++)
    {
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

// std::cout << "All height tests passed!\n";

    std::cout << "\nAll tests passed!\n";

    return 0;
}