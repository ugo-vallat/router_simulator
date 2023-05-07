#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>


#include "tab_rout.h"




int main(void) {
    Table t = table_import("../INPUT/config_router_1.txt");
    printf(" >>> [test create + test add + import] table created\n\n");
    table_display(t);
    printf(" >>> [format to send] retour : \n");
    Packet packet = table_to_packet(t);
    packet_display(packet);
    packet_delete(packet);
    table_export(t, "../OUTPUT/output_test.txt");
    table_delete(t);
    printf(" >>> [test delete] table deleted\n\n");
    return 0;
}