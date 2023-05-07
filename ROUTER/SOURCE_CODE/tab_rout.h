#ifndef __TAB_ROUT_H__
#define __TAB_ROUT_H__

#include "packet.h"


#define SIZE_IP_NET 15
#define SIZE_IP_ROUT 15
#define SIZE_MASK_ROUT 3
#define SIZE_WEIGHT 3

typedef struct s_table *Table ;

Table table_create();

bool table_add(Table t, char* ipNet, char* ipRout, int maskRout, int weight);

bool table_add_block(Table t, const Block b);

int table_size(const Table t);

void table_delete(Table t);

void table_display(const Table t);

Packet table_to_packet(const Table t);

void table_export(const Table t, char* path);

Table table_import(char* path);






#endif