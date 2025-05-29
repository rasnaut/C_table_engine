typedef struct BinaryTreeNode {
    char* key; // Changed from 'data' to 'key' for clarity
    char* data;
    struct BinaryTreeNode* left;
    struct BinaryTreeNode* right;
} BinaryTreeNode;

typedef struct BinaryTree {
    struct BinaryTreeNode* root;
    size_t size;
    size_t height;
} BinaryTree;

BinaryTree* binary_tree_create();

int binary_tree_insert(BinaryTree* tree, const char* key, const char* data);
void binary_tree_destroy(BinaryTree* tree);
BinaryTreeNode* binary_tree_search(const BinaryTree* tree, const char* key);
int binary_tree_get_all_nodes_contained_substring(
    const BinaryTreeNode* tree, const char* substring, BinaryTree* result_tree);
void binary_tree_erase_node_by_key(BinaryTree* tree, const char* key);
void binary_tree_print(const BinaryTree* tree);
void binary_tree_print_sort_view(const BinaryTree* tree);
size_t binary_tree_calculate_height(const BinaryTree *node);

size_t binary_tree_node_calculate_height_recursiv(const BinaryTreeNode *node);
BinaryTreeNode* binary_tree_node_create(const char* key, const char* data);
int binary_tree_node_create_child_recursive(BinaryTreeNode* parent, const char* key, const char* data, size_t *height);
BinaryTreeNode* binary_tree_node_erase_child_by_key(BinaryTreeNode* tree, const char* key);
void binary_tree_node_destroy(BinaryTreeNode* node);
void binary_tree_node_recursive_destroy(BinaryTreeNode* node);
void binary_tree_node_print(const BinaryTreeNode* node, int level);
void binary_tree_node_recursive_print_tree_view(const BinaryTreeNode* node, int level);
void binary_tree_node_recursive_print_sort_view(const BinaryTreeNode* node);

