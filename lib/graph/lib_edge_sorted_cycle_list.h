#ifndef LIB_EDGE_SORTED_CYCLE_LIST_H
#define LIB_EDGE_SORTED_CYCLE_LIST_H

#include <stddef.h>
#include "lib_vertex_handle.h"
#include "lib_edge.h"

// Кольцевой двусвязный список рёбер, отсортированный по edge_compare()
typedef struct EdgeSortedCycleList {
    Edge*  head; // NULL => пусто; head->prev — последний элемент
    size_t size;
} EdgeSortedCycleList;

// Инициализация пустого списка
void edge_list_init(EdgeSortedCycleList* list);

// Освобождает все рёбра в списке (edge_dispose + free каждого узла)
// После вызова список пуст
void edge_list_dispose(EdgeSortedCycleList* list);

// Вставка или обновление.
// Если ребро к тому же VertexHandle уже есть — обновляем его weight и освобождаем node (edge_dispose+free).
// Если ребра нет — вставляем node по сортировке, список принимает владение node.
// is_update!=NULL → *is_update=1 при обновлении, 0 при вставке.
// Возврат: 0 — ок, -1 — ошибка аргументов.
int edge_list_upsert(EdgeSortedCycleList* list, Edge* node, int* is_update);

// Удаляет конкретный узел из списка и освобождает его (edge_dispose + free)
// Возвращает 0 — успех, -1 — ошибка (узел не найден/список пуст)
int edge_list_remove(EdgeSortedCycleList* list, Edge* node);

// Ищет ребро по handle назначения
// Возвращает указатель на Edge или NULL, не меняет список
Edge* edge_list_find_by_handle(const EdgeSortedCycleList* list, const VertexHandle* to);

// Поиск по имени назначенной вершины (точное совпадение).
Edge* edge_list_find_by_name(const EdgeSortedCycleList* list, const char* name);

// Удаляет ребро по handle назначения (если найдено — освобождает)
// Возвращает 0 — успех, -1 — не найдено
int edge_list_remove_by_handle(EdgeSortedCycleList* list, const VertexHandle* to);

// Удаляет все рёбра, у которых handle невалиден (to==NULL или to->vertex==NULL).
// Возвращает количество удалённых рёбер через removed_count (если не NULL).
int edge_list_remove_invalid(EdgeSortedCycleList* list, size_t* removed_count);

// Проверка пустоты
int edge_list_is_empty(const EdgeSortedCycleList* list);

// Возвращает количество рёбер
size_t edge_list_size(const EdgeSortedCycleList* list);

// Отладочная печать всех рёбер
void edge_list_print(const EdgeSortedCycleList* list);

#endif // LIB_EDGE_SORTED_CYCLE_LIST_H
