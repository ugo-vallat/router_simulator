#ifndef __LIST_H__
#define __LIST_H__

#define FREE_DATA true
#define KEEP_DATA false

/*--------------------------------------------------------------*/

typedef struct s_List* List;

typedef bool(*fequal)(void*, void*) ;

typedef void(*fmap)(void* data, void* buff) ;

List list_create();

List list_add(List l, void* elem);

void* list_at(List l, int pos);

int list_size(const List l);

int list_searsh_pos(List l, fequal f, void* elem);

List list_remove(List l, int pos, bool freeData);

void list_delete(List l, bool freeData);

void list_map(const List l, fmap f, void* buff);



#endif