#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include "packet.h"



int main(void) {
    printf("\n[ test create ] : \n");
    Packet p = packet_create();
    printf("\n[ test construct ] : \n");
    packet_add_block(p, "2.2.2.2         6.6.6.6         2      7  ");
    packet_add_block(p, "2.33.33.2       6.6.6.6         2      7  ");
    packet_add_block(p, "783.781.756.951 745.7.327.7     38     198");
    printf("\n[ test display + size ] : \n");
    packet_display(p);
    printf("\n[ test at ] : pos 1\n");
    Block b = packet_block_at(p, 1);
    printf("Block = >%s<\n",b);
    printf("\n[ test delete ] : \n");
    packet_delete(p);
    printf("Packet delete\n");

}