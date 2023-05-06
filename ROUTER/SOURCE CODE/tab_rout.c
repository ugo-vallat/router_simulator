
#include "tab_rout.h"
#include <stdio.h>
#include <math.h>



#define TWO_POW24 16777216
#define TWO_POW16 65536
#define TWO_POW8 256

/*------------------------------------------ STRUCT ------------------------------------------*/

typedef struct s_entry *Entry;

struct s_entry {
    char ipNet[SIZE_IP_NET+1];
    int maskRout;
    char ipRout[SIZE_IP_ROUT+1];
    int weight;
};


struct s_table {
    List list;
};


/*------------------------------------------ CREATE ------------------------------------------*/

Entry entry_create(char* ipNet, char* ipRout, int maskRout, int weight) {
    Entry new = malloc(sizeof(struct s_entry));
    strcpy(new->ipNet, ipNet);
    strcpy(new->ipRout, ipRout);
    new->maskRout = maskRout;
    new->weight = weight;
    return new;
}

Table table_create() {
    Table t = malloc(sizeof(struct s_table));
    t->list = list_create();
    return t;
}


/*------------------------------------------ ADD ------------------------------------------*/

bool compare_ip(void* entry, void* ip) {
    return !((bool)(strcmp((char*)ip, ((Entry)entry)->ipNet)));
}

char* apply_mask(char* ipFull , int mask) {
    char* ipNet = malloc(sizeof(char)*16);
    strcpy(ipNet, ipFull);
    int v1, v2, v3, v4;
    uint32_t ip_bin = 0x0;
    uint32_t mask_bin = 0xFFFFFFFF;
    sscanf(ipNet, "%d.%d.%d.%d", &v1, &v2, &v3, &v4);
    ip_bin = (v1 << 24) | (v2 << 16) | (v3 << 8) | v4;
    mask_bin = mask_bin << (32-mask);
    ip_bin = ip_bin & mask_bin;
    sprintf(ipNet,"%u.%u.%u.%u",
        (ip_bin >> 24) & 0xFF,
        (ip_bin >> 16) & 0xFF,
        (ip_bin >> 8) & 0xFF,
        ip_bin & 0xFF);
    return ipNet;
}


bool table_add(Table t, char* ipNet, char* ipRout, int maskRout, int weight) {
    int pos;
    Entry new;
    char* ip_mask = apply_mask(ipNet, maskRout);
    if((pos = list_searsh_pos(t->list, compare_ip,(void*)ip_mask)) != -1) {
        new = list_at(t->list, pos);
        if(new->weight > weight) {
           new = entry_create(ip_mask, ipRout, maskRout, weight);
           list_remove(t->list, pos, FREE_DATA);
           list_add(t->list, (void*)new);
           return true;
        }
        return false;
    }
    new = entry_create(ip_mask, ipRout, maskRout, weight);
    list_add(t->list, (void*)new);
    free(ip_mask);
    return true;   
}


bool table_add_block(Table t, const Block b) {
    char ipNet[16];
    char ipRout[16];
    int maskRout, weight;
    sscanf(b, "%s %d %s %d", ipNet, &maskRout, ipRout, &weight);
    return table_add(t, ipNet, ipRout, maskRout, weight);
}



/*------------------------------------------ SIZE ------------------------------------------*/

int table_size(const Table t) {
    return list_size(t->list);
}


/*------------------------------------------ DELETE ------------------------------------------*/

void table_delete(Table t) {
    list_delete(t->list, FREE_DATA);
    free(t);
}


/*------------------------------------------ DISPLAY ------------------------------------------*/

void print_entry(void* data, void* buff) {
    (void)buff;
    Entry entry = (Entry)data;
    printf(" %15s | %3d | %15s | %3d \n", entry->ipNet, entry->maskRout, entry->ipRout, entry->weight);
}

void table_display(const Table t) {
    printf("____________________________________________________\n    IP NETWORK   | MASK|    IP ROUTER    | WEIGHT \n");
    list_map(t->list, print_entry, NULL);
    printf("\n");

}


/*------------------------------------------ FORMAT TO SEND ------------------------------------------*/


void add_to_packet(void* value, void* buff) {
    Packet packet = (Packet)buff;
    Entry entry = (Entry)value;
    char block[SIZE_BLOCK];
    sprintf(block, "%-15s %-3d %-15s %-3d", entry->ipNet, entry->maskRout, entry->ipRout, entry->weight);
    packet_add_block(packet, block);    
}


Packet table_to_packet(const Table t) {
    Packet packet = packet_create();
    list_map(t->list, add_to_packet, packet);
    return packet;
}



/*------------------------------------------ EXPORT ------------------------------------------*/

void print_output(void* entry, void* output) {
    fprintf((FILE*)output, "%-15s %-3d %-15s %-3d\n", ((Entry)entry)->ipNet, ((Entry)entry)->maskRout,((Entry)entry)->ipRout, 
     ((Entry)entry)->weight);
}


void table_export(const Table t, char* path) {
    FILE *output;
    output = fopen(path, "w");
    if(output == NULL) {
        perror("fopen error");
        return;
    }
    fprintf(output, "%d\n", list_size(t->list));
    list_map(t->list, print_output, output);
}


/*------------------------------------------ IMPORT ------------------------------------------*/

Table table_import(char* path) {
    FILE *input;
    Table table;
    int nbEntry;

    input = fopen(path, "r");
    if(input == NULL) {
        perror("fopen error");
        return NULL;
    }
    
    if(fscanf(input, "%d\n", &nbEntry) == -1) {
        perror("fscanf error");
        exit(EXIT_FAILURE);
    }

    table = table_create();

    for(int i = 0; i < nbEntry; i++) {
        char ipNet[SIZE_IP_NET+1], ipRout[SIZE_IP_ROUT+1];
        int maskRout, weight;
        if(fscanf(input, "%s %d %s %d\n", ipNet, &maskRout, ipRout, &weight) == -1 ) {
            perror("fscanf error");
            exit(EXIT_FAILURE);
        }
        table_add(table, ipNet, ipRout, maskRout, weight);
    }
    table_display(table);
    fclose(input);
    return table;
}


