template <typename T>
struct Node
{
    T value;
    Node* left;
    Node* right;

    Node(T val): value(val), left(nullptr), right(nullptr)
    {};
};