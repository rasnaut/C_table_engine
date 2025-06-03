typedef struct BinaryTreeNode {
    char* key; // Changed from 'data' to 'key' for clarity
    char* data;
    struct BinaryTreeNode* parent;
    struct BinaryTreeNode* left;
    struct BinaryTreeNode* right;
} BinaryTreeNode;

size_t binary_tree_node_calculate_height_recursiv(const BinaryTreeNode *node);
BinaryTreeNode* binary_tree_node_create(const char* key, const char* data);
int binary_tree_node_create_child_recursive(BinaryTreeNode *parent, const char *key, const char *data, size_t *height);
int binary_tree_node_create_child_iterative(BinaryTreeNode *parent, const char *key, const char *data, size_t *height);
BinaryTreeNode * binary_tree_node_create_child(BinaryTreeNode *parent, const char *key, const char *data);
BinaryTreeNode* binary_tree_node_erase_child_by_key(BinaryTreeNode* tree, const char* key);
BinaryTreeNode* binary_tree_node_erase_child_by_key_iterative(BinaryTreeNode* root, const char* key);
void binary_tree_node_destroy(BinaryTreeNode* node);
void binary_tree_node_recursive_destroy(BinaryTreeNode* node);
BinaryTreeNode *binary_tree_node_search_node_by_key(const BinaryTreeNode *tree, const char *key);
void binary_tree_node_print(const BinaryTreeNode* node, int level);
void binary_tree_node_recursive_print_tree_view(const BinaryTreeNode* node, int level);
void binary_tree_node_recursive_print_sort_view(const BinaryTreeNode* node);