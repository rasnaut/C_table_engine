struct BinaryTree {
    BinaryTreeNode* root;
    size_t size;
    size_t height;
};

struct BinaryTreeNode {
    char* key; // Changed from 'data' to 'key' for clarity
    char* data;
    BinaryTreeNode* left;
    BinaryTreeNode* right;
};

BinaryTree* binary_tree_create();

int binary_tree_insert(BinaryTree* tree, const char* key, const char* data);
void binary_tree_destroy(BinaryTree* tree);
BinaryTreeNode* binary_tree_search(const BinaryTree* tree, const char* key);
int binary_tree_get_all_nodes_contained_substring(
    const BinaryTree* tree, const char* substring, BinaryTree* result_tree);
void binary_tree_print(const BinaryTree* tree);

BinaryTreeNode* binary_tree_node_create(const char* key, const char* data);
int binary_tree_node_create_child_recursive(BinaryTreeNode* parent, const char* key, const char* data);
void binary_tree_node_destroy(BinaryTreeNode* node);
void binary_tree_node_recursive_destroy(BinaryTreeNode* node);
void binary_tree_node_print(const BinaryTreeNode* node, int level);
void binary_tree_node_recursive_print_tree_view(const BinaryTreeNode* node, int level);
void binary_tree_node_recursive_print_sort_view(const BinaryTreeNode* node);
