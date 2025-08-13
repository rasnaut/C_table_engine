#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib_vertex.h"
#include "lib_edge.h"
#include "lib_edge_sorted_cycle_list.h"
#include "lib_vertex_handle.h"
#include "lib_graph.h"

// --- helpers ---
static int clamp_w(int w){ if (w < -10) return -10; if (w > 10) return 10; return w; }

// ========== VertexHandle ==========
static void test_vh_lifecycle() {
    Vertex* v = vertex_create("VH");
    assert(v && v->handle);
    VertexHandle* h = v->handle;

    // базовое состояние
    assert(!vertex_handle_is_null(h));
    // добавим 2 «внешние» ссылки (как будто два ребра)
    assert(vertex_handle_add_ref(h) == 0);
    assert(vertex_handle_add_ref(h) == 0);

    // invalidation не должен мешать release
    vertex_handle_invalidate(h);
    assert(vertex_handle_is_null(h));

    // два внешних release
    size_t rc = vertex_handle_release(h);
    (void)rc;
    rc = vertex_handle_release(h);
    (void)rc;

    // теперь релиз вершины (её +1)
    vertex_dispose(v); // внутри произойдёт финальный release handle
    // сюда просто дойдём без краша/утечек
}

// ========== Edge ==========
static void test_edge_refcount_and_print() {
    Vertex* a = vertex_create("A");
    Vertex* b = vertex_create("B");
    assert(a && b);

    Edge* e = (Edge*)malloc(sizeof(Edge));
    assert(e);
    assert(edge_init(e, b->handle, 7) == 0);

    // у handle B стало на +1 ref больше
    // (точное значение зависит от внутренней инициализации, но release не должен падать)
    edge_print(e); // должен печатать "B, 7" и не падать

    // инвалидируем B и убедимся, что печать корректна
    vertex_dispose(b);
    edge_print(e); // печатает "(invalid), 7" — не падает

    edge_dispose(e);
    free(e);
    vertex_dispose(a);
}

// ========== EdgeSortedCycleList ==========
static void test_escl_basic_sort_upsert_find() {
    Vertex* a = vertex_create("A");
    Vertex* b = vertex_create("Bob");
    Vertex* c = vertex_create("Carol");

    EdgeSortedCycleList L;
    edge_list_init(&L);

    // вставим в перемешанном порядке
    Edge* e_bc = (Edge*)malloc(sizeof(Edge)); assert(e_bc);
    assert(edge_init(e_bc, c->handle, -2) == 0);
    int is_update = 0;
    assert(edge_list_upsert(&L, e_bc, &is_update) == 0 && is_update == 0);

    Edge* e_bb = (Edge*)malloc(sizeof(Edge)); assert(e_bb);
    assert(edge_init(e_bb, b->handle, 5) == 0);
    assert(edge_list_upsert(&L, e_bb, &is_update) == 0 && is_update == 0);

    // порядок должен быть Bob, Carol (по имени)
    assert(L.size == 2);
    assert(strcmp(L.head->to->vertex->name_of_person, "Bob") == 0);
    assert(strcmp(L.head->next->to->vertex->name_of_person, "Carol") == 0);

    // upsert: добавляем дубликат к Bob — должен обновиться вес и освобождаться переданный узел
    Edge* e_dup = (Edge*)malloc(sizeof(Edge)); assert(e_dup);
    assert(edge_init(e_dup, b->handle, 9) == 0);
    assert(edge_list_upsert(&L, e_dup, &is_update) == 0 && is_update == 1);
    // убедимся, что у существующего ребра к Bob вес стал 9
    Edge* eb = edge_list_find_by_name(&L, "Bob"); assert(eb && eb->weight == 9);

    // поиск по handle
    Edge* ec = edge_list_find_by_handle(&L, c->handle); assert(ec == L.head->next);

    // удаление по handle
    assert(edge_list_remove_by_handle(&L, c->handle) == 0);
    assert(L.size == 1);
    assert(strcmp(L.head->to->vertex->name_of_person, "Bob") == 0);

    edge_list_dispose(&L);
    vertex_dispose(a);
    vertex_dispose(b);
    vertex_dispose(c);
}

// ========== Vertex (интеграция) ==========
static void test_vertex_add_upsert_remove_and_invalidation() {
    Vertex* alice = vertex_create("Alice");
    Vertex* bob   = vertex_create("Bob");
    Vertex* carol = vertex_create("Carol");
    assert(alice && bob && carol);

    // add A->B (5)
    assert(vertex_add_edge(alice, bob->handle, 5) == 0);
    // upsert A->B (-3) => вес обновится
    assert(vertex_add_edge(alice, bob->handle, -3) == 0);

    // add A->C (100) — будет зажат до 10
    assert(vertex_add_edge(alice, carol->handle, 100) == 0);

    // проверим состояние списка: Bob (-3), Carol (10)
    assert(edge_list_size(&alice->edges) == 2);
    Edge* e1 = alice->edges.head;
    Edge* e2 = e1->next;
    // порядок по имени: Bob затем Carol
    assert(strcmp(e1->to->vertex->name_of_person, "Bob") == 0);
    assert(e1->weight == -3);
    assert(strcmp(e2->to->vertex->name_of_person, "Carol") == 0);
    assert(e2->weight == 10);

    // удалим Carol как вершину → handle инвалидируется
    vertex_dispose(carol);

    // сметём все рёбра с невалидными хендлами
    size_t removed = 0;
    assert(edge_list_remove_invalid(&alice->edges, &removed) == 0);
    assert(removed == 1);

    // удалим ребро к Bob по handle
    assert(vertex_remove_edge(alice, bob->handle) == 0);
    assert(edge_list_is_empty(&alice->edges));

    vertex_dispose(bob);
    vertex_dispose(alice);
}


static void free_string_array(char** arr, size_t n){
    if (!arr) return; for (size_t i=0;i<n;++i) free(arr[i]); free(arr);
}

static void free_components(GraphComponents* comps){
    if (!comps) return;
    for (size_t i=0;i<comps->num_components;++i){
        if (comps->names && comps->names[i]){
            for (size_t j=0;j<comps->component_sizes[i]; ++j) free(comps->names[i][j]);
            free(comps->names[i]);
        }
    }
    free(comps->component_sizes);
    free(comps->names);
    free(comps);
}

// --- helpers to check membership ignoring order ---
static int name_in_array(const char* name, char** arr, size_t n){
    for (size_t i=0;i<n;++i) if (strcmp(arr[i], name)==0) return 1; return 0;
}

static int array_equals_set(char** arr, size_t n, const char** expect, size_t m){
    if (n!=m) return 0; for (size_t i=0;i<m;++i) if (!name_in_array(expect[i], arr, n)) return 0; return 1;
}

static int component_contains_all(const GraphComponents* comps, const char** names, size_t n){
    if (!comps) return 0;
    for (size_t i=0;i<comps->num_components;++i){
        size_t hit=0; for (size_t j=0;j<n;++j) if (name_in_array(names[j], comps->names[i], comps->component_sizes[i])) hit++;
        if (hit==n) return 1; // нашли компоненту, содержащую все требуемые имена
    }
    return 0;
}

// --- build a demo graph ---
static Graph* build_demo_graph(void){
    Graph* g = graph_create();
    assert(g);

    // vertices
    Vertex* alice = graph_add_vertex(g, "Alice");
    Vertex* bob   = graph_add_vertex(g, "Bob");
    Vertex* carol = graph_add_vertex(g, "Carol");
    Vertex* dave  = graph_add_vertex(g, "Dave");
    Vertex* eve   = graph_add_vertex(g, "Eve");
    Vertex* frank = graph_add_vertex(g, "Frank");
    Vertex* gina  = graph_add_vertex(g, "Gina");

    assert(alice && bob && carol && dave && eve && frank && gina);

    // relations (weights)
    // Главный кластер: Alice->Bob(5), Bob->Carol(1), Bob->Eve(10), Eve->Dave(2), Carol->Dave(-3)
    assert(vertex_add_edge(alice, bob->handle,   5) == 0);
    assert(vertex_add_edge(bob,   carol->handle, 1) == 0);
    assert(vertex_add_edge(bob,   eve->handle,  10) == 0);
    assert(vertex_add_edge(eve,   dave->handle,  2) == 0);
    assert(vertex_add_edge(carol, dave->handle, -3) == 0);

    // Сделаем цикл для SCC: Eve->Bob(3)
    assert(vertex_add_edge(eve,   bob->handle,   3) == 0);

    // Отдельный мини-кластер: Frank->Gina(3)
    assert(vertex_add_edge(frank, gina->handle,  3) == 0);

    return g;
}

// ============ TESTS ============
static void test_graph_add_find_remove_vertex(){
    Graph* g = graph_create();
    assert(g && graph_is_empty(g));

    Vertex* a = graph_add_vertex(g, "Alice");
    Vertex* b = graph_add_vertex(g, "Bob");
    assert(graph_size(g) == 2);
    assert(graph_find_vertex(g, "Alice") == a);
    assert(graph_find_vertex(g, "Bob")   == b);
    assert(graph_find_vertex(g, "Carol") == NULL);

    // повторное добавление возвращает существующую
    Vertex* a2 = graph_add_vertex(g, "Alice");
    assert(a2 == a);

    // удаление и проверка
    assert(graph_remove_vertex(g, "Alice") == 0);
    assert(graph_find_vertex(g, "Alice") == NULL);
    assert(graph_size(g) == 1);

    graph_dispose(g);
}

static void test_graph_bfs_within_k(){
    Graph* g = build_demo_graph();

    char** names=NULL; size_t count=0;

    // K=1, любые рёбра: из Alice достижим только Bob
    assert(graph_bfs_within_k(g, "Alice", 1, 0, &names, &count) == 0);
    const char* exp1[] = {"Bob"};
    assert(array_equals_set(names, count, exp1, 1));
    free_string_array(names, count);

    // K=2, только положительные: из Alice достижимы Bob, Carol, Eve
    assert(graph_bfs_within_k(g, "Alice", 2, 1, &names, &count) == 0);
    const char* exp2[] = {"Bob", "Carol", "Eve"};
    assert(array_equals_set(names, count, exp2, 3));
    free_string_array(names, count);

    // K=3, только положительные: теперь добавится Dave (через Eve)
    assert(graph_bfs_within_k(g, "Alice", 3, 1, &names, &count) == 0);
    const char* exp3[] = {"Bob", "Carol", "Eve", "Dave"};
    assert(array_equals_set(names, count, exp3, 4));
    free_string_array(names, count);

    graph_dispose(g);
}

static void test_graph_shortest_positive_chain(){
    Graph* g = build_demo_graph();

    char** path=NULL; size_t path_len=0; int total_cost=0;

    // Путь Alice -> Dave по положительным рёбрам:
    // Alice->Bob(5), Bob->Eve(10), Eve->Dave(2)
    assert(graph_shortest_positive_chain(g, "Alice", "Dave", 0, &path, &path_len, &total_cost) == 0);
    assert(path_len == 4); // Alice, Bob, Eve, Dave
    const char* exp_names[] = {"Alice","Bob","Eve","Dave"};
    assert(array_equals_set(path, path_len, exp_names, 4));
    free_string_array(path, path_len);

    // Стоимость в режиме cost_mode=1: (11-5)+(11-10)+(11-2) = 6+1+9 = 16
    assert(graph_shortest_positive_chain(g, "Alice", "Dave", 1, &path, &path_len, &total_cost) == 0);
    assert(path_len == 4 && total_cost == 16);
    free_string_array(path, path_len);

    // Недостижимость: Dave -> Alice (нет положительных обратных рёбер к Alice)
    assert(graph_shortest_positive_chain(g, "Dave", "Alice", 0, &path, &path_len, &total_cost) == 0);
    assert(path == NULL && path_len == 0);

    graph_dispose(g);
}

static void test_graph_positive_components(){
    Graph* g = build_demo_graph();

    GraphComponents* undirected = NULL;
    assert(graph_positive_components(g, 0, &undirected) == 0);

    // Ожидаем 2 компоненты: {Alice,Bob,Carol,Eve,Dave} и {Frank,Gina}
    assert(undirected && undirected->num_components == 2);
    const char* big_comp[] = {"Alice","Bob","Carol","Eve","Dave"};
    const char* small_comp[] = {"Frank","Gina"};
    assert(component_contains_all(undirected, big_comp, 5));
    assert(component_contains_all(undirected, small_comp, 2));
    free_components(undirected);

    GraphComponents* scc = NULL;
    assert(graph_positive_components(g, 1, &scc) == 0);

    // В SCC ожидаем компоненту {Bob, Eve} (есть взаимные положительные рёбра), остальные по одиночке
    assert(scc && scc->num_components >= 3);
    const char* scc_pair[] = {"Bob","Eve"};
    assert(component_contains_all(scc, scc_pair, 2));
    free_components(scc);

    graph_dispose(g);
}

// ========== main ==========
int main(void) {
    // test_vh_lifecycle();
    // test_edge_refcount_and_print();
    // test_escl_basic_sort_upsert_find();
    // test_vertex_add_upsert_remove_and_invalidation();

    test_graph_add_find_remove_vertex();
    test_graph_bfs_within_k();
    test_graph_shortest_positive_chain();
    test_graph_positive_components();

    printf("All tests passed.\n");
    return 0;
}
