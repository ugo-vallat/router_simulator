
#include "tab_rout.h"




int main(void) {
    Table t = table_import("../TEST/input_test.txt");
    printf(" >>> [test create + test add + import] table created\n\n");
    table_display(t);
    printf(" >>> [display] table expected : \n");
    printf("____________________________________________________\n\
    IP NETWORK   |    IP ROUTER    |  PORT  | WEIGHT \n\
         2.2.2.2 |         6.6.6.6 |      2 |   7 \n\
         1.1.1.1 |         7.7.7.7 |      3 |   1 \n\
         1.1.2.2 |         5.5.5.5 |      1 |   2 \n\n");
    printf(" >>> [format to send] retour : \n");
    Packet packet = table_to_packet(t);
    packet_display(packet);
    packet_delete(packet);
    table_export(t, "../TEST/output_test.txt");
    table_delete(t);
    printf(" >>> [test delete] table deleted\n\n");
    return 0;
}