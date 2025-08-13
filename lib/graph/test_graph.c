#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib_vertex.h"
#include "lib_edge.h"
#include "lib_edge_sorted_cycle_list.h"
#include "lib_vertex_handle.h"

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

// ========== main ==========
int main(void) {
    test_vh_lifecycle();
    test_edge_refcount_and_print();
    test_escl_basic_sort_upsert_find();
    test_vertex_add_upsert_remove_and_invalidation();

    printf("All tests passed.\n");
    return 0;
}
