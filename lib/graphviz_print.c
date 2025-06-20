
#include <stdio.h>
#include <stdlib.h>
#include "graphviz_print.h"

void node234_generate_dot(const Node234* root, const char* filename) {
    if (!root || !filename) return;

    FILE* file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", filename);
        return;
    }

    fprintf(file, "digraph Tree234 {\n");
    fprintf(file, "    node [shape=record, style=filled, fillcolor=lightgoldenrod];\n");

    const Node234* stack[256];
    int top = 0;
    stack[top++] = root;

    while (top > 0) {
        const Node234* node = stack[--top];

        // Построим label вида: <c0> | key0 | <c1> | key1 | <c2> | key2 | <c3>
        fprintf(file, "    \"%p\" [label=\"", (void*)node);

        for (int i = 0; i <= node->key_count; ++i) {
            fprintf(file, "<c%d>", i); // вход для связи с дочерним узлом
            if (i < node->key_count) {
                fprintf(file, "|%s", node->elements[i].key);
            }
            if (i < node->key_count) fprintf(file, "|");
        }
        fprintf(file, "\"];\n");

        for (int i = 0; i <= node->key_count; ++i) {
            if (i < node->child_count && node->children[i]) {
                fprintf(file, "    \"%p\":c%d -> \"%p\";\n", (void*)node, i, (void*)node->children[i]);
                if (top < 256) stack[top++] = node->children[i];
            }
        }
    }

    fprintf(file, "}\n");
    fclose(file);
    printf("✅ Graphviz dot-файл сгенерирован: %s\n", filename);
}

