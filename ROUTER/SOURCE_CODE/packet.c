#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "linked_list.h"
#include "packet.h"


struct s_Packet {
    int size;
    List data;
};



/*------------------------------------------ CREATE ------------------------------------------*/

Packet packet_create() {
    Packet new = malloc(sizeof(struct s_Packet));
    new->size = 0;
    new->data = list_create();
    return new;
}



/*------------------------------------------ ADD ------------------------------------------*/

Packet packet_add_block(Packet p, const Block data) {
    char* block = malloc(sizeof(char)*(strlen(data)+1));
    strcpy(block, data);
    list_add(p->data, block);
    p->size++;
    return p;
}


/*------------------------------------------ SIZE ------------------------------------------*/

int packet_size(Packet p) {
    return p->size;
}


/*------------------------------------------ AT ------------------------------------------*/

Block packet_block_at(Packet p, int i) {
    return (Block)list_at(p->data, i);
}


/*------------------------------------------ DELETE ------------------------------------------*/

void packet_delete(Packet p) {
    list_delete(p->data, FREE_DATA);
    free(p);
}


/*------------------------------------------ DISPLAY ------------------------------------------*/

void print_packet(void* data, void* buff) {
    (void)buff;
    printf(">%s<\n", (Block)data);
}

void packet_display(Packet p) {
    printf(" >>> PACKET (%d) <<<\n", p->size);
    list_map(p->data, print_packet, NULL);
    printf("\n");
}