#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#include "lib/dialogue.h"

static int ensure_graph(const Graph* g) {
    if (!g) {
        printf("❌ Graph is not created yet. Use option 1 first.\n");
        return 0;
    }
    return 1;
}

int main(void) {
    Graph* graph = NULL;

    for (;;) {
        unsigned int i = 1;
        printf("\n==== People Graph ====\n");
        printf("%u - Create graph\n", i++);
        printf("%u - Add vertex (person)\n", i++);
        printf("%u - Add/Update edge (relationship)\n", i++);
        printf("%u - Edit vertex (rename)\n", i++);
        printf("%u - Edit edge (change weight)\n", i++);
        printf("%u - Delete vertex\n", i++);
        printf("%u - Delete edge\n", i++);
        printf("%u - Print adjacency lists\n", i++);
        printf("%u - BFS within K handshakes\n", i++);
        printf("%u - Shortest positive-only path (Dijkstra)\n", i++);
        printf("%u - Positive components (undirected/SCC)\n", i++);
        printf("%u - Generate Graphviz .dot\n", i++);
        printf("%u - Import graph from file\n", i++);
        printf("%u - Exit\n", i++);

        char* inp = readline("Enter command number: ");
        if (!inp) break;
        int cmd = atoi(inp);
        free(inp);

        switch (cmd) {
            case 1: { // create
                if (graph) {
                    printf("⚠ Graph already exists. Disposing old and creating new.\n");
                    graph_dispose(graph);
                }
                graph = dialogue_create_graph();
            } break;

            case 2: { // add vertex
                if (!ensure_graph(graph)) break;
                dialogue_add_vertex(graph);
            } break;

            case 3: { // add/update edge
                if (!ensure_graph(graph)) break;
                dialogue_add_or_update_edge(graph);
            } break;

            case 4: { // edit vertex (rename)
                if (!ensure_graph(graph)) break;
                dialogue_edit_vertex(graph);
            } break;

            case 5: { // edit edge (change weight)
                if (!ensure_graph(graph)) break;
                dialogue_edit_edge(graph);
            } break;

            case 6: { // delete vertex
                if (!ensure_graph(graph)) break;
                dialogue_delete_vertex(graph);
            } break;

            case 7: { // delete edge
                if (!ensure_graph(graph)) break;
                dialogue_delete_edge(graph);
            } break;

            case 8: { // print adjacency
                if (!ensure_graph(graph)) break;
                dialogue_print_adjacency(graph);
            } break;

            case 9: { // BFS within K
                if (!ensure_graph(graph)) break;
                dialogue_bfs_within_k(graph);
            } break;

            case 10: { // shortest positive path
                if (!ensure_graph(graph)) break;
                dialogue_shortest_positive_path(graph);
            } break;

            case 11: { // components
                if (!ensure_graph(graph)) break;
                dialogue_print_positive_components(graph);
            } break;

            case 12: { // graphviz
                if (!ensure_graph(graph)) break;
                dialogue_generate_graphviz(graph);
            } break;

            case 13: { // import
                if (!ensure_graph(graph)) {
                    // если графа нет — создадим автоматически
                    graph = dialogue_create_graph();
                    if (!graph) break;
                }
                dialogue_import_graph(graph);
            } break;

            case 14: { // exit
                if (graph) {
                    graph_dispose(graph);
                    graph = NULL;
                }
                printf("Exiting...\n");
                return 0;
            } break;

            default:
                printf("Error. No such operation\n");
                break;
        }
    }

    if (graph) {
        graph_dispose(graph);
        graph = NULL;
    }
    return 0;
}
