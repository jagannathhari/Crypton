typedef struct Node {
    char *file_name;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
}List;

Node *new_node(char *data);
void print_list(List *list);
void free_node(Node *node);
void clear_list(Node *head);
void add_node(List *list, char *name);
void remove_node(Node *current , Node *previous,List *list);
void free_all_nodes(struct Node* head);
