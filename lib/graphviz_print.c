
#include <stdio.h>
#include <stdlib.h>
#include "graphviz_print.h"

void binary_tree_generate_dot(const BinaryTree *tree, const char *filename) {
    if (!tree || !filename) return;

    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return;
    }

    fprintf(file, "digraph BinaryTree {\n");
    fprintf(file, "    node [shape=record, style=filled, fillcolor=lightblue];\n");

    BinaryTreeNode* stack[256];
    int top = 0;
    if (tree->root) {
        stack[top++] = tree->root;
    }

    while (top > 0) {
        BinaryTreeNode* node = stack[--top];

        // Ограничим длину текста
        char key[64], data[64];
        snprintf(key, sizeof(key), "%.60s", node->key);
        snprintf(data, sizeof(data), "%.60s", node->data);

        fprintf(file, "    \"%p\" [label=\"{<f0> %s | <f1> %s}\"];\n",
                (void*)node, key, data);

        if (node->left) {
            fprintf(file, "    \"%p\":f0 -> \"%p\";\n", (void*)node, (void*)node->left);
            if (top < 256) stack[top++] = node->left;
        }
        if (node->right) {
            fprintf(file, "    \"%p\":f1 -> \"%p\";\n", (void*)node, (void*)node->right);
            if (top < 256) stack[top++] = node->right;
        }
    }

    fprintf(file, "}\n");
    fclose(file);
    printf("Graphviz dot-файл сгенерирован: %s\n", filename);
}
