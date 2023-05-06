

#include "linked_list.h"

#define DEFAULT_BOOL_VALUE(expr, default_val) ((expr) ? (expr) : (default_val))


typedef struct s_Node *Node;

struct s_Node {
    void* data;
    Node previous;
    Node next;
};

struct s_List {
    int size;
    Node sentinel;
};


/*------------------------------------------ CREATE ------------------------------------------*/

List list_create(){
    List l = malloc(sizeof(struct s_List));
    Node sent = malloc(sizeof(struct s_Node));

    sent->data = NULL;
    sent->next = sent->previous = sent;

    l->sentinel = sent;
    l->size = 0;

    return l;
}


/*------------------------------------------ ADD ------------------------------------------*/

List list_add(List l, void* elem) {
    Node new = malloc(sizeof(struct s_Node));
    new->data = elem;
    new->next = l->sentinel;
    new->previous = l->sentinel->previous;

    l->sentinel->previous = new;
    new->previous->next = new;

    l->size++;
    return l;
}


/*------------------------------------------ AT ------------------------------------------*/

void* list_at(List l, int pos) {
    if (pos < 0 || pos >= l->size) return NULL;
    Node cur = l->sentinel->next;
    for(int i = 0; i < pos; i++)
        cur = cur->next;
    return cur->data;
}


/*------------------------------------------ SIZE ------------------------------------------*/

int list_size(const List l) {
    return l->size;
}


/*------------------------------------------ SEARSH POS ------------------------------------------*/

int list_searsh_pos(List l, fequal f, void* elem) {
    Node cur = l->sentinel->next;
    for(int i = 0; i < l->size; i++) {
        if (f(cur->data, elem)) return i;
        cur = cur->next;
    }
    return -1;
}


/*------------------------------------------ REMOVE ------------------------------------------*/

List list_remove(List l, int pos, bool freeData) {
    if (pos < 0 || pos >= l->size) return NULL;
    Node cur = l->sentinel->next;
    for(int i = 0; i < pos; i++) cur = cur->next;
    cur->next->previous = cur->previous;
    cur->previous->next = cur->next;
    l->size--;
    if(freeData) free(cur->data);
    free(cur);
    return l;
}




/*------------------------------------------ DELETE ------------------------------------------*/

void list_delete(List l, bool freeData) {
    Node cur = l->sentinel->next;
    while(cur != l->sentinel) {
        cur = cur->next;
        if(freeData) free(cur->previous->data);
        free(cur->previous);
    }
    free(l->sentinel);
    free(l);
}


void list_map(const List l, fmap f, void* buff) {
    for(Node cur = l->sentinel->next; cur != l->sentinel; cur = cur->next)
        f(cur->data, buff);
}


