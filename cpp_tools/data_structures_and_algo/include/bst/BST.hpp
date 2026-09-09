#include "Node.hpp"
#include <cstdio>
#include <iostream>
#include <optional>
#include <queue>
// #include <fmt/core.h>
#include <concepts>

#pragma once


template <typename T>requires std::totally_ordered<T>
// doesn't allow duplicates
class BST
{
private:
    Node<T>* root;

    // helper functions (recursive core logic)
private:
    // get smallest value starting at the current node
    Node<T>* findMin(Node<T>* node) const;
    // get largest value starting at the current node
    Node<T>* findMax(Node<T>* node) const;
    // Implemented with level-order traversal algorithm
    int height(Node<T>* node) const;
    Node<T>* insert(Node<T>* node, T value);
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
    Node<T>* remove(Node<T>* node, T value);
    bool contains(Node<T>* node, T value) const;

    void inorder(Node<T>* node) const;
    void preorder(Node<T>* node) const;
    void postorder(Node<T>* node) const;

    void destroy(Node<T>* node);

    // core API
public:
    BST();
    BST(T value);
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





template <typename T> requires std::totally_ordered<T>
Node<T>* BST<T>::findMin(Node<T>* node) const
{
    if (!node) return nullptr;
    while (node->left)
    {
        node = node->left;
    }

    return node;
}

template <typename T>requires std::totally_ordered<T>
Node<T>* BST<T>::findMax(Node<T>* node) const
{
    if (!node) return nullptr;
    while (node->right)
    {
        node = node->right;
    }

    return node;
}

template <typename T>requires std::totally_ordered<T>
int BST<T>::height() const
{
    return height(this->root);
}


/*
50
/    \
30      70
/  \    /  \
20   40  60   80
/          \     \
10           65     90
*/
template <typename T>requires std::totally_ordered<T>
int BST<T>::height(Node<T>* node) const
{
    int lvl = -1;
    if (!node) return lvl;
    std::queue<Node<T>*> q;
    q.push(node);
    q.push(nullptr);
    while (!q.empty())
    {
        auto poppedNode = q.front();
        q.pop();
        if (!poppedNode)
        {
            lvl+=1;
            // only need to push a null node if q still has nodes left
            if (!q.empty()) q.push(nullptr);
            continue;
        }
        if (poppedNode->left) q.push(poppedNode->left);
        if (poppedNode->right) q.push(poppedNode->right);
    }

    return lvl;
}

template <typename T>requires std::totally_ordered<T>
void BST<T>::remove(T value)
{
    this->root = this->remove(this->root, value);
}


template <typename T>requires std::totally_ordered<T>
void BST<T>::insert(T value)
{
    this->root = this->insert(this->root, value);
}

template <typename T>requires std::totally_ordered<T>
std::optional<T> BST<T>::max() const
{
    auto node = this->findMax(this->root);
    if (!node)
    {
        return std::nullopt;
    }
    return node->value;
}


template <typename T>requires std::totally_ordered<T>
std::optional<T> BST<T>::min() const
{
    auto node = this->findMin(this->root);
    if (!node)
    {
        return std::nullopt;
    }
    return node->value;
}

template <typename T>requires std::totally_ordered<T>
void BST<T>::printInOrder() const
{
    this->inorder(this->root);
    printf("\n");
}

template <typename T>requires std::totally_ordered<T>
void BST<T>::printPreOrder() const
{
    this->preorder(this->root);
    printf("\n");
}

template <typename T>requires std::totally_ordered<T>
void BST<T>::printPostOrder() const
{
    this->postorder(this->root);
    printf("\n");
}

template <typename T>requires std::totally_ordered<T>
bool BST<T>::empty() const
{
    return this->root == nullptr;
}

template <typename T>requires std::totally_ordered<T>
BST<T>::BST()
{
    root = nullptr;
}

template <typename T>requires std::totally_ordered<T>
BST<T>::~BST()
{
    destroy(this->root);
    this->root = nullptr;
}

template <typename T>requires std::totally_ordered<T>
void BST<T>::destroy(Node<T>* node)
{
    if (node == nullptr)
    {
        return;
    }

    destroy(node->left);
    destroy(node->right);
    node->left = nullptr;
    node->right = nullptr;
    delete node;
}

template <typename T>requires std::totally_ordered<T>
BST<T>::BST(T value)
{
    this->root = new Node<T>(value);
}

template <typename T>requires std::totally_ordered<T>
Node<T>* BST<T>::insert(Node<T>* node, T value)
{
    if (node == nullptr)
        return new Node<T>(value);

    if (value < node->value)
        node->left = insert(node->left, value);
    else if (value > node->value)
        node->right = insert(node->right, value);
    // else: duplicate, do nothing

    return node;
}


/*
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
*/
template <typename T>requires std::totally_ordered<T>
Node<T>* BST<T>::remove(Node<T>* node, T value)
{
    if (!node)
        return nullptr;
    if (value < node->value)
    {
        node->left = remove(node->left, value);
        return node;
    }
    else if (value > node->value)
    {
        node->right = remove(node->right, value);
        return node;
    }

    // handle 3 cases
    if (!node->left && !node->right)
    {
        delete node;
        return nullptr;
    }

    // std::cout << "node to delete: " << node->value << std::endl;

    // std::cout << "node left is null: " << (node->left == nullptr) << std::endl;
    // std::cout << "node right is null: " << (node->right == nullptr) << std::endl;
    

    if (height(node->left) > height(node->right))
    {
        // go for inorder predecessor
        Node<T>* predecessor = findMax(node->left);
        node->value = predecessor->value;
        node->left = remove(node->left, predecessor->value);
        return node;
    }
    else
    {
        // go for inorder predecessor
        // printf("hi\n");

        Node<T>* successor = findMin(node->right);
        // printf("hi\n");

        // printf("successor: %d\n", successor == nullptr);
        // printf("node->value: %d\n", node->value);


        node->value = successor->value;
        node->right = remove(node->right, successor->value);
        return node;
    }

}

template <typename T>requires std::totally_ordered<T>
void BST<T>::preorder(Node<T>* node) const
{
    if (node == nullptr)
    {
        return;
    }

    // fmt::print("{} ",node->value);

    std::cout << node->value << " " << std::endl;
    preorder(node->left);
    preorder(node->right);
}

template <typename T>requires std::totally_ordered<T>
void BST<T>::postorder(Node<T>* node) const
{
    if (node == nullptr)
    {
        return;
    }

    postorder(node->left);
    postorder(node->right);
    // fmt::print("{} ",node->value);
    std::cout << node->value << " ";

}

template <typename T>requires std::totally_ordered<T>
void BST<T>::inorder(Node<T>* node) const
{
    if (node == nullptr)
    {
        return;
    }

    inorder(node->left);
    // fmt::print("{} ",node->value);
    std::cout << node->value << " " ;//<< std::endl;
    inorder(node->right);
}

template <typename T>requires std::totally_ordered<T>
bool BST<T>::contains(T value) const
{
    return this->contains(this->root, value);
}

template <typename T>requires std::totally_ordered<T>
bool BST<T>::contains(Node<T>* node, T value) const
{
    if (node == nullptr)
        return false;

    if (node->value == value)
    {
        return true;
    }
    
    return contains(node->left, value) || contains(node->right, value);
}


