typedef struct Vertex;

typedef struct VertexHandle {
    Vertex* vertex;
    size_t ref_count; // счётчик ссылок на хендлер вершины
} VertexHandle;

int vertex_handle_init(VertexHandle* handle, Vertex* vertex);
int vertex_handler_is_null(const VertexHandle* handle);
int vertex_handler_add_ref(VertexHandle* handle);
int vertex_handler_remove_ref_or_free(VertexHandle* handle);

typedef struct Edge {
    VertexHandle* to; // указатель на хендлер вершины, к которой ведёт ребро
    int weight;     // вес ребра
    struct Edge* next;
    struct Edge* prev; // указатель на предыдущее ребро в списке смежных рёбер
} Edge;

int edge_init(Edge* edge, VertexHandle* to, int weight);
int edge_free(Edge* edge);
int edge_print(const Edge* edge);
int edge_compare(const Edge* edge1, const Edge* edge2);

typedef struct EdgeSortedCycleList {
    Edge* head; // указатель на первое ребро в списке
    size_t size; // количество рёбер в списке
} EdgeSortedCycleList;

EdgeSortedCycleList* edge_list_create();
int edge_list_insert(EdgeSortedCycleList* edge, Edge* new_edge);
int edge_list_remove(EdgeSortedCycleList* edge, Edge* edge_to_remove);

typedef struct Vertex {
    char* name_of_person;    // имя человека
    Edge* edges;             // список смежных рёбер
    VertexHandle* handle; // указатель на хендлер вершины
} Vertex;

Vertex* vertex_create(const char* name_of_person);
int vertex_free(Vertex* vertex);
int vertex_print(const Vertex* vertex);
int vertex_add_edge(Vertex* vertex, VertexHandle* to, int weight);
int vertex_remove_edge(Vertex* vertex, VertexHandle* to);
int vertex_remove_edge_by_name(Vertex* vertex, const char* name_of_person);

