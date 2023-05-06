
#include "linked_list.h"

void print_data(void *data, void *buff) {
    (void)buff;
    printf("%d ", *((int*)data));
}

void test_add(List l) {
    for(int i = 0; i < 11; i++) {
        int *val = malloc(sizeof(int));
        *val = i;
        list_add(l, val);
    }
    list_map(l, print_data, NULL);
    printf("\n");
}

void test_at(List l) {
    for(int i = 0; i < 11; i++) {
        printf("%d ", *((int*)list_at(l, i)));
    }
    printf("\n");
}

bool isEqual(void* data, void* val) {
    return *((int*)data) == *((int*)val);
}

void test_searsh(List l) {
    for(int i = -1; i < 12; i++) {
        int val = i;
        printf("%d ", list_searsh_pos(l, isEqual, (void*)&val));
    }
    printf("\n");
}

void test_remove(List l) {
    for(int i = 5; i >= 0; i--) {
        list_remove(l, i, FREE_DATA);
    }
    list_map(l, print_data, NULL);
    printf("\n");
}


int main (void) {
    List l = list_create();
    printf(" >>> [test create] list created\n\n");
    test_add(l);
    printf(" >>> [test add] expected : \n0 1 2 3 4 5 6 7 8 9 10\n\n");
    test_at(l);
    printf(" >>> [test at] expected : \n0 1 2 3 4 5 6 7 8 9 10\n\n");
    printf("Size = %d\n >>> [test size] expected :\nSize = 11\n\n", list_size(l));
    test_searsh(l);
    printf(" >>> [test searsh] expected : \n-1 0 1 2 3 4 5 6 7 8 9 10 -1\n\n");
    test_remove(l);
    printf(" >>> [test remove] expected : \n6 7 8 9 10\n\n");
    list_delete(l, FREE_DATA);
    printf(" >>> [test delete] list delete\n\n");
    return 0;
}