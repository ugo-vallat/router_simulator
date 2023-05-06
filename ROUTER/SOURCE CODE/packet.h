#ifndef __PACKET_H__
#define __PACKET_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "linked_list.h"

#define SIZE_BLOCK 64
#define SIZE_BUFF_BLOCK (SIZE_BLOCK*sizeof(char))

typedef struct s_Packet *Packet;

typedef char* Block;

Packet packet_create();

Packet packet_add_block(Packet p, const Block data);

int packet_size(Packet p);

Block packet_block_at(Packet p, int i);

void packet_delete(Packet p);

void packet_display(Packet p);


#endif