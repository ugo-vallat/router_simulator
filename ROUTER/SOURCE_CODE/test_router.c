#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

#include "linked_list.h"
#include "packet.h"
#include "router.h"

#define NUMBER_ROUTER_MAX 8
#define LOCALHOST "127.0.0.1"

typedef struct s_Pair *Pair;

typedef struct s_IpRout *IpRout;

typedef struct s_DataRout *DataRout ;

struct s_Pair {
    int v1;
    int v2;
};

struct s_IpRout{
    int router;
    char* ip;
    int port;
};

struct s_DataRout {
    int num;
    int port_rout;
    char* ip_phy;
    char* ip_vir;
    int* port_next_rout;
    int nb_next_rout;
};


bool search_pair(void* elem, void* to_search) {
    Pair pair = (Pair)elem;
    Pair pair_search = (Pair)to_search;
    return(((pair->v1 == pair_search->v1) && (pair->v2 == pair_search->v2)) ||
    ((pair->v1 == pair_search->v2) && (pair->v2 == pair_search->v1)));
}


List make_list_connexion(FILE* input) {
    List list_pair = list_create();
    int nb_read = 1;
    Pair new_pair;

    while(nb_read != EOF){
        new_pair = malloc(sizeof(struct s_Pair));
        nb_read = fscanf(input, "%d - %d", &new_pair->v1, &new_pair->v2);
        if(nb_read != EOF && (list_searsh_pos(list_pair, search_pair, new_pair) == -1) )
            list_add(list_pair, new_pair);
        else free(new_pair);
    }
    return list_pair;
}

List make_list_ip_rout(FILE* input) {
    List list_ip_rout = list_create();
    IpRout new;
    int number_router;

    if(fscanf(input, "%d", &number_router) == -1){
        perror("[make_list_ip_rout] read number of rout error");
        list_delete(list_ip_rout, FREE_DATA);
        exit(EXIT_FAILURE);
    }
    if(number_router>NUMBER_ROUTER_MAX) {
        fprintf(stderr, "[test_router] too many router : max = %d\n", NUMBER_ROUTER_MAX);
        list_delete(list_ip_rout, FREE_DATA);
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < number_router; i++) {
        new = malloc(sizeof(struct s_IpRout));
        new->ip = malloc(32);
        if(fscanf(input, "%d - %s - %d", &(new->router), new->ip, &(new->port))==-1){
            perror("[make_list_ip_rout] read info router error");
            list_delete(list_ip_rout, FREE_DATA);
            exit(EXIT_FAILURE);
        }
        list_add(list_ip_rout, new);
    }
    return list_ip_rout;

}

bool searsh_port_of_rout(void* elem, void* num) {
    return ((IpRout)elem)->router == *((int*)num);
}

int get_port_of_router(int num, List list_ip_rout) {
    int pos = list_searsh_pos(list_ip_rout, searsh_port_of_rout, &num);
    if(pos == -1) {
        fprintf(stderr, "[get_ip_of_rout] Impossible connexion : router n°%d doesn't exist\n", num);
        exit(EXIT_FAILURE);
    }
    return (((IpRout)list_at(list_ip_rout, pos))->port);
}


DataRout make_list_next_rout(List list_ip_rout, List list_connexion, DataRout data_rout) {
    data_rout->port_next_rout = malloc(sizeof(int)*NUMBER_ROUTER_MAX);
    Pair pair;
    for(int i = 0; i < list_size(list_connexion); i++) {
        pair = list_at(list_connexion, i);
        if(pair->v1 == data_rout->num) {
            data_rout->port_next_rout[data_rout->nb_next_rout] = get_port_of_router(pair->v2,list_ip_rout);
            data_rout->nb_next_rout++;
        } else if(pair->v2 == data_rout->num) {
            data_rout->port_next_rout[data_rout->nb_next_rout] = get_port_of_router(pair->v1,list_ip_rout);
            data_rout->nb_next_rout++;
        }
    }
    return data_rout;
}


List make_list_data_rout(List list_ip_rout, List list_connexion) {
    List list_data_rout = list_create();
    DataRout data;
    IpRout ip_rout;
    for(int i = 0; i < list_size(list_ip_rout); i++) {
        data = malloc(sizeof(struct s_DataRout));
        data->nb_next_rout = 0;
        ip_rout = list_at(list_ip_rout, i);
        data->num = ip_rout->router;
        data->port_rout = ip_rout->port;
        data->ip_phy = LOCALHOST;
        data->ip_vir = ip_rout->ip;
        data = make_list_next_rout(list_ip_rout, list_connexion, data);
        list_add(list_data_rout, data);
    }
    return list_data_rout;
}

void delete_all(List list_ip_rout, List list_connexion, List list_data_rout){
    list_delete(list_ip_rout, FREE_DATA);
    list_delete(list_connexion, FREE_DATA);
    list_delete(list_data_rout, FREE_DATA);
}

void print_ip_rout(void* elem, void* none) {
    IpRout ip_rout = (IpRout)elem;
    printf("router %d : ip = %s, port = %d\n", ip_rout->router, ip_rout->ip, ip_rout->port);
    (void)none;
}


void print_connexion(void* elem, void* none) {
    Pair pair = (Pair)elem;
    printf("router %d <=> router %d\n", pair->v1, pair->v2);
    (void)none;
}


void print_data(void* elem, void* none) {
    DataRout data = (DataRout)elem;
    printf("Router %d : \n  - ip_phy = %s\n    - port_rout = %d\n    - port_next_rout = %d\n    - nb_next_rout = %d\n    - ip_vir = %s\n"
    ,data->num, data->ip_phy, data->port_rout, data->port_next_rout[0], data->nb_next_rout, data->ip_vir);
    (void)none;
}

int main(void) {
    List list_ip_rout;
    List list_connexion;
    List list_data_rout;
    FILE* config_network;
    DataRout data;
    int status;

    config_network = fopen("../INPUT/config_network.txt", "r");
    if(config_network == NULL) {
        perror("open config_network.txt error");
        exit(EXIT_FAILURE);
    }
    list_ip_rout = make_list_ip_rout(config_network);
    list_connexion = make_list_connexion(config_network);
    fclose(config_network);
    list_data_rout = make_list_data_rout(list_ip_rout, list_connexion);



    for (int i = 0; i < list_size(list_data_rout); i++) {
        if((status = fork())== -1) {
            perror("fork error");
            delete_all(list_ip_rout, list_connexion, list_data_rout);
            exit(EXIT_FAILURE);
        } else if (status == 0) {
            data = (DataRout)list_at(list_data_rout, i);
            router_main(data->num, data->ip_phy, data->port_rout, data->port_next_rout, data->nb_next_rout, data->ip_vir);
            exit(0);
        }
    
    }
    while (wait(NULL) != -1) {
        printf("[main] end process\n");
    }
    delete_all(list_ip_rout, list_connexion, list_data_rout);



    return 0;
}