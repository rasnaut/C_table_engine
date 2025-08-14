#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>

#include "dialogue.h"
#include "graph/lib_vertex.h"
#include "graph/lib_edge.h"
#include "graph/lib_edge_sorted_cycle_list.h"
#include "graphviz_print.h"

// --- helpers ---

static char* ask(const char* prompt) {
    char* s = readline(prompt);
    if (!s) return NULL;
    // обрежем хвостовые \r\n если вдруг попали (на всякий)
    size_t n = strlen(s);
    while (n && (s[n-1]=='\n' || s[n-1]=='\r')) s[--n] = '\0';
    return s;
}

static int parse_int(const char* s, int* out) {
    if (!s || !*s) return -1;
    char* end = NULL;
    long v = strtol(s, &end, 10);
    if (end == s || *end != '\0') return -1;
    //if (v < INT_MIN || v > INT_MAX) return -1;
    *out = (int)v;
    return 0;
}

static int clamp_weight(int w) {
    if (w < -10) return -10;
    if (w > 10)  return 10;
    return w;
}

static Vertex* find_vertex_by_name(Graph* g, const char* name) {
    if (!g || !name) return NULL;
    return graph_find_vertex(g, name);
}

// --- dialogue commands ---

Graph* dialogue_create_graph(void) {
    Graph* g = graph_create();
    if (!g) {
        printf("❌ Failed to create graph\n");
        return NULL;
    }
    printf("✅ Graph created\n");
    return g;
}

int dialogue_add_vertex(Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* name = ask("Enter person name: ");
    if (!name) return -1;
    Vertex* v = graph_add_vertex(g, name);
    if (!v) {
        printf("❌ Failed to add/find vertex\n");
        free(name);
        return -1;
    }
    printf("✅ Vertex present: %s\n", v->name_of_person);
    free(name);
    return 0;
}

int dialogue_add_or_update_edge(Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* from = ask("From person: ");
    char* to   = ask("To person: ");
    char* ws   = ask("Weight [-10..10]: ");
    if (!from || !to || !ws) { free(from); free(to); free(ws); return -1; }

    int w;
    if (parse_int(ws, &w) != 0) { printf("❌ Bad weight\n"); free(from); free(to); free(ws); return -1; }
    w = clamp_weight(w);

    Vertex* vf = find_vertex_by_name(g, from);
    Vertex* vt = find_vertex_by_name(g, to);
    if (!vf || !vt) {
        printf("❌ Unknown person(s)\n");
        free(from); free(to); free(ws);
        return -1;
    }

    int rc = vertex_add_edge(vf, vt->handle, w); // upsert внутри
    if (rc == 0) printf("✅ Edge %s -> %s weight=%d set\n", from, to, w);
    else         printf("❌ Failed to set edge\n");

    free(from); free(to); free(ws);
    return rc;
}

int dialogue_edit_vertex(Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* name = ask("Current name: ");
    if (!name) return -1;

    Vertex* v = graph_find_vertex(g, name);
    if (!v) { printf("❌ Person not found\n"); free(name); return -1; }

    char* new_name = ask("New name: ");
    if (!new_name) { free(name); return -1; }

    char* dup = strdup(new_name);
    if (!dup) { printf("❌ OOM\n"); free(name); free(new_name); return -1; }

    free(v->name_of_person);
    v->name_of_person = dup;

    // Примечание: порядок в списках исходящих рёбер других вершин
    // не пересобирается специально после переименования.
    // Он обновится при дальнейших вставках/удалениях.

    printf("✅ Renamed '%s' → '%s'\n", name, new_name);
    free(name);
    free(new_name);
    return 0;
}

int dialogue_edit_edge(Graph* g) {
    // просто делегируем на add_or_update
    return dialogue_add_or_update_edge(g);
}

int dialogue_delete_vertex(Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* name = ask("Person to delete: ");
    if (!name) return -1;
    int rc = graph_remove_vertex(g, name);
    if (rc == 0) printf("✅ Vertex '%s' deleted\n", name);
    else         printf("❌ Failed to delete '%s'\n", name);
    free(name);
    return rc;
}

int dialogue_delete_edge(Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* from = ask("From person: ");
    char* to   = ask("To person: ");
    if (!from || !to) { free(from); free(to); return -1; }

    Vertex* vf = find_vertex_by_name(g, from);
    Vertex* vt = find_vertex_by_name(g, to);
    if (!vf || !vt) {
        printf("❌ Unknown person(s)\n");
        free(from); free(to);
        return -1;
    }
    int rc = vertex_remove_edge(vf, vt->handle);
    if (rc == 0) printf("✅ Edge %s -> %s removed\n", from, to);
    else         printf("❌ Failed to remove edge\n");
    free(from); free(to);
    return rc;
}

void dialogue_print_adjacency(const Graph* g) {
    if (!g || !g->head || g->size == 0) {
        printf("(empty graph)\n");
        return;
    }
    const VertexNode* vnode = g->head;
    for (size_t i = 0; i < g->size; ++i) {
        const Vertex* v = vnode->v;
        printf("%s:", v->name_of_person);
        if (!v->edges.head || v->edges.size == 0) {
            printf(" (no outgoing)\n");
        } else {
            const Edge* e = v->edges.head;
            for (size_t k = 0; k < v->edges.size; ++k) {
                const char* to_name = "(invalid)";
                if (e->to && e->to->vertex && e->to->vertex->name_of_person)
                    to_name = e->to->vertex->name_of_person;
                printf("  [%s, w=%d]", to_name, e->weight);
                e = e->next;
            }
            printf("\n");
        }
        vnode = vnode->next;
    }
}

int dialogue_bfs_within_k(Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* start = ask("Start person: ");
    char* ks    = ask("Max hops (K): ");
    char* ps    = ask("Positive only? (0/1): ");
    if (!start || !ks || !ps) { free(start); free(ks); free(ps); return -1; }

    int k_i=0, pos=0;
    if (parse_int(ks, &k_i) != 0 || k_i < 0) { printf("❌ Bad K\n"); free(start); free(ks); free(ps); return -1; }
    if (parse_int(ps, &pos) != 0) { printf("❌ Bad flag\n"); free(start); free(ks); free(ps); return -1; }

    char** names = NULL;
    size_t count = 0;
    int rc = graph_bfs_within_k(g, start, (size_t)k_i, pos ? 1 : 0, &names, &count);
    if (rc != 0) {
        printf("❌ BFS failed (start not found?)\n");
        free(start); free(ks); free(ps);
        return -1;
    }
    printf("Reachable within %d hop(s)%s from '%s':\n",
           k_i, pos ? " (positive-only)" : "", start);
    for (size_t i = 0; i < count; ++i) {
        printf("  - %s\n", names[i]);
        free(names[i]);
    }
    free(names);
    free(start); free(ks); free(ps);
    return 0;
}

int dialogue_shortest_positive_path(Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* src = ask("Source person: ");
    char* dst = ask("Target person: ");
    char* ms  = ask("Cost mode (0=min hops, 1=11-weight): ");
    if (!src || !dst || !ms) { free(src); free(dst); free(ms); return -1; }

    int mode=0;
    if (parse_int(ms, &mode) != 0 || (mode!=0 && mode!=1)) {
        printf("❌ Bad mode\n"); free(src); free(dst); free(ms); return -1;
    }

    char** path = NULL;
    size_t path_len = 0;
    int total_cost = 0;
    int rc = graph_shortest_positive_chain(g, src, dst, mode, &path, &path_len, &total_cost);
    if (rc != 0) {
        printf("❌ Dijkstra failed (persons exist?)\n");
        free(src); free(dst); free(ms);
        return -1;
    }
    if (path_len == 0) {
        printf("No positive-only path from '%s' to '%s'\n", src, dst);
    } else {
        printf("Path (%zu): ", path_len);
        for (size_t i = 0; i < path_len; ++i) {
            printf("%s%s", path[i], (i+1<path_len) ? " -> " : "");
            free(path[i]);
        }
        free(path);
        if (mode == 1) printf("\nTotal cost: %d", total_cost);
        printf("\n");
    }
    free(src); free(dst); free(ms);
    return 0;
}

int dialogue_print_positive_components(Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* ds = ask("Directed mode? (0=undirected positive connectivity, 1=SCC positive): ");
    if (!ds) return -1;
    int directed=0;
    if (parse_int(ds, &directed) != 0 || (directed!=0 && directed!=1)) {
        printf("❌ Bad mode\n"); free(ds); return -1;
    }
    GraphComponents* comps = NULL;
    int rc = graph_positive_components(g, directed, &comps);
    if (rc != 0 || !comps) { printf("❌ Components failed\n"); free(ds); return -1; }

    printf("Components (%zu):\n", comps->num_components);
    for (size_t i = 0; i < comps->num_components; ++i) {
        printf("  Group %zu:", i+1);
        for (size_t j = 0; j < comps->component_sizes[i]; ++j) {
            printf(" %s", comps->names[i][j]);
            free(comps->names[i][j]);
        }
        printf("\n");
        free(comps->names[i]);
    }
    free(comps->names);
    free(comps->component_sizes);
    free(comps);
    free(ds);
    return 0;
}

int dialogue_generate_graphviz(const Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* out = ask("Output .dot filename: ");
    if (!out) return -1;
    graph_generate_dot(g, out);
    printf("Tip: dot -Tpng %s -o out.png\n", out);
    free(out);
    return 0;
}

int dialogue_import_graph(Graph* g) {
    if (!g) { printf("❌ Graph not initialized\n"); return -1; }
    char* path = ask("Import file path: ");
    if (!path) return -1;

    FILE* f = fopen(path, "r");
    if (!f) { printf("❌ Cannot open file\n"); free(path); return -1; }

    char line[1024];
    size_t line_no = 0;
    int rc = 0;

    while (fgets(line, sizeof(line), f)) {
        ++line_no;
        // пропустим комментарии/пустые
        if (line[0]=='#' || line[0]=='\n' || line[0]=='\r') continue;

        // уберём \n
        size_t n = strlen(line);
        while (n && (line[n-1]=='\n' || line[n-1]=='\r')) line[--n] = '\0';

        if (line[0]=='V' && line[1]==' ') {
            char* name = line + 2;
            if (!*name) { printf("line %zu: empty vertex name\n", line_no); rc = -1; break; }
            if (!graph_add_vertex(g, name)) { printf("line %zu: add vertex failed\n", line_no); rc=-1; break; }
        } else if (line[0]=='E' && line[1]==' ') {
            // E from to weight
            char from[256], to[256];
            int w;
            if (sscanf(line+2, "%255s %255s %d", from, to, &w) != 3) {
                printf("line %zu: bad edge line\n", line_no); rc=-1; break;
            }
            Vertex* vf = graph_find_vertex(g, from);
            Vertex* vt = graph_find_vertex(g, to);
            if (!vf || !vt) {
                printf("line %zu: unknown vertex in edge\n", line_no); rc=-1; break;
            }
            w = clamp_weight(w);
            if (vertex_add_edge(vf, vt->handle, w) != 0) {
                printf("line %zu: add edge failed\n", line_no); rc=-1; break;
            }
        } else {
            printf("line %zu: unknown directive (use 'V name' or 'E from to weight')\n", line_no);
            rc = -1; break;
        }
    }

    fclose(f);
    if (rc == 0) printf("✅ Import done: %s\n", path);
    free(path);
    return rc;
}
