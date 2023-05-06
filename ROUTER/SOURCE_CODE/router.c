#define _POSIX_C_SOURCE 200809L

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/select.h>


#include "packet.h"
#include "tab_rout.h"
#include "router.h"

#define TIME_READ_MAX 5
#define TIME_TO_LIVE_MAX 10
#define NB_RECEIV_MAX 512
#define NB_TRANSMITE_MAX 512
#define NB_MODIF_TABLE_MAX 512




/*------------------------------------------ ERROR FUNCTION ------------------------------------------*/

void error(char* message, int exit_value) {
    perror(message);
    exit(exit_value);
}


void error_recv(char* message, int value_exit, int socket_rout, int tube, Block buff) {
    free(buff);
    close(socket_rout);
    close(tube);
    error(message, value_exit);
}


void error_tran(char* message, int value_exit, int socket_rout, int tube, Block buff, struct sockaddr_in *addr_dest) {
    free(addr_dest);
    free(buff);
    close(socket_rout);
    close(tube);
    error(message, value_exit);
}


void error_tab(char* message, int value_exit, int tube_send, int tube_recv, Table table, Block buff) {
    free(buff);
    close(tube_recv);
    close(tube_send);
    table_delete(table);
    error(message, value_exit);
}

void error_router(char* message, int value_exit, int tube_send[2], int tube_recv[2], Table table) {
    close(tube_recv[0]); close(tube_recv[1]);
    close(tube_send[0]); close(tube_send[1]);
    table_delete(table);
    error(message, value_exit);
}





/*------------------------------------------ RECEIVER ------------------------------------------*/

void router_receiver(int num, char* ip_phy, int port,  int tube) {
    struct sockaddr_in addr_re, addr_se;
    socklen_t addr_se_len = sizeof(addr_se);
    int socket_rout;
    Block buff = malloc(SIZE_BUFF_BLOCK);
    fd_set readfds;
    struct timeval timeout;
    int ready;
    char message[BUFSIZ]; 

    /* scocket creation */
// printf("[router %d] Creation socket recv\n", num);
    socket_rout = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_rout == -1) {
        close(tube);
        error("[router_receiver] socket error", EXIT_FAILURE);
    }

    /* Adress creation */
    memset(&addr_re, 0, sizeof(addr_re));
    addr_re.sin_family =AF_INET;
    addr_re.sin_port = htons(port);
    addr_re.sin_addr.s_addr = inet_addr(ip_phy);
    
    /* Bind socket */
    if(bind(socket_rout, (const struct sockaddr *)&addr_re, sizeof(addr_re)) == -1) {
        error_recv("[router_receiver] bind error", EXIT_FAILURE, socket_rout, tube, buff);
    }

// signal(SIGALRM, process_killer);
// alarm(TIME_TO_LIVE_MAX);
    FD_ZERO(&readfds);
    FD_SET(socket_rout, &readfds);

    for(int i = 0; i < NB_RECEIV_MAX; i++) {
        /* set timeout to 10 seconds */
        timeout.tv_sec = TIME_READ_MAX;
        timeout.tv_usec = 0;

        /* wait something on socket */
// printf("[router_receiver] Waiting block\n");
        ready = select(socket_rout + 1, &readfds, NULL, NULL, &timeout);
        if(ready > 0) { /* if something */
            /* read block on socket */
// printf("[router_receiver] read block in socket\n");
            if(recvfrom(socket_rout, buff, SIZE_BUFF_BLOCK, 0, (struct sockaddr *)&addr_se, &addr_se_len) == -1) {
                error_recv("[router_receiver] recvfrom error", EXIT_FAILURE, socket_rout, tube, buff);
            }
// if(num == 3) printf("[receiver 3] new bloc : %s\n", buff);

            /* send block to the table */
// printf("[router receiver %d] Received bloc : %s\n",num , block);
            if(write(tube, buff, SIZE_BUFF_BLOCK) == -1) {
                error_recv("[router_receiver] write to table error", EXIT_FAILURE, socket_rout, tube, buff);
            }
        } else {
            /* no more block received */
            sprintf(message,"[router_receiver %d] No more packets received", num);
            error_recv(message, EXIT_FAILURE, socket_rout, tube, buff);
        }
    }
    /* max operations reached */
    sprintf(message,"[router_receiver %d] number max operations reached", num);
    error_recv(message, EXIT_FAILURE, socket_rout, tube, buff);
}







/*------------------------------------------ TRANSMITTER ------------------------------------------*/


struct sockaddr_in * init_addr(char* ip_phy, int *port, int nb_port) {
    struct sockaddr_in * addr_dest = malloc(sizeof(struct sockaddr_in)*nb_port);
    for(int i = 0; i < nb_port; i++) {
        addr_dest[i].sin_family = AF_INET;
        addr_dest[i].sin_addr.s_addr = inet_addr(ip_phy);
        addr_dest[i].sin_port = htons(port[i]);
    }
    return addr_dest;
}

void send_all(const struct sockaddr_in* addr, int nb_addr, const int socket_send,const Block block) {
    // printf("[router_transmitter] sending block %s\n", block);
    for(int i = 0; i < nb_addr; i++) {
        if(sendto(socket_send, block, SIZE_BUFF_BLOCK, 0, (const struct sockaddr *)&(addr[i]), sizeof(addr[i])) == -1) {
            close(socket_send);
            error("[router_transmitter] sendto error", EXIT_FAILURE);
        }
    }
}


void router_transmitter(int num, char* ip_phy, int *port, int nb_port, int tube) {
    struct sockaddr_in *addr_dest;
    int socket_rout;
    Block buff = malloc(SIZE_BUFF_BLOCK);
    fd_set readfds;
    struct timeval timeout;
    int ready, nb_read;
    char message[BUFSIZ]; 

    socket_rout = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket_rout == -1) {
        error("[router_transmitter] open socket transmitter error", EXIT_FAILURE);
        close(tube);
    }

    addr_dest = init_addr(ip_phy, port, nb_port);
// printf("[router_transmitter] Ready \n");
// signal(SIGALRM, process_killer);
// alarm(TIME_TO_LIVE_MAX);

    FD_ZERO(&readfds);
    FD_SET(tube, &readfds);

    for(int i = 0; i < NB_TRANSMITE_MAX; i++) {
        // set timeout to 10 seconds
        timeout.tv_sec = TIME_READ_MAX;
        timeout.tv_usec = 0;
        // printf("[router_transmitter] Waiting block to send\n");
        ready = select(tube + 1, &readfds, NULL, NULL, &timeout);
        // printf("[router_transmitter] Read in pip_phye block to send\n");
        if (ready != 0 && (nb_read = read(tube, buff, SIZE_BUFF_BLOCK)) != 0) {
            if(nb_read == -1) {
                error_tran("[router_transmitter] read block from table error", EXIT_FAILURE, socket_rout, tube, buff, addr_dest);
            }
// if(num == 3) printf("[transmitter 3] bloc send : %s\n", buff);
            send_all(addr_dest, nb_port, socket_rout,buff);
        } else {
            sprintf(message,"[router_transmitter %d] No more block to send", num);
            error_tran(message, EXIT_FAILURE, socket_rout, tube, buff, addr_dest);

        }
        
    }
    sprintf(message,"[router_transmitter %d] number max operation reached", num);
    error_tran(message, EXIT_FAILURE, socket_rout, tube, buff, addr_dest);
}





/*------------------------------------------ TABLE ------------------------------------------*/

char* add_ip_vir_and_weight(Block block, char* ip_vir) {
    char ip_net[16], ip_root[16];
    int mask_root, weight;
    sscanf(block, "%s %d %s %d", ip_net, &mask_root, ip_root, &weight);
    sprintf(block,"%15s %3d %15s %3d", ip_net, mask_root, ip_vir, weight+1);
    return block;
}


void send_block(int tube, Block block) {
    if(write(tube, block, SIZE_BUFF_BLOCK) == -1) {
        close(tube);
        free(block);
        error("[router_table] send block to transmitter error  ", EXIT_FAILURE);
    }
}

void send_packet(int tube, Packet packet, char* ip_vir) {
// printf("[router table] send packet to transmitter\n");
// packet_display(packet);
    Block block;
    for (int i = 0; i < packet_size(packet); i++) {
        block = packet_block_at(packet, i);
        add_ip_vir_and_weight(block, ip_vir);
        send_block(tube, block);
    }
    packet_delete(packet);
}

char* create_path_export(int num) {
    char *path = malloc(sizeof(char)*BUFSIZ);
    sprintf(path, "../OUTPUT/table_routeur_%d.txt", num);
    return path;
}

void export_table_router(Table table, int num) {
    char* path_export = create_path_export(num);
// if(num == 3) table_display(table);
    table_export(table, path_export);
// if(num == 3) printf(" >>> table exported\n");
    free(path_export);
}

void router_table(int num, char* ip_vir, Table table, int tube_send, int tube_recv) {
    sleep(1);
    Block buff = malloc(SIZE_BUFF_BLOCK);
    fd_set readfds;
    struct timeval timeout;
    int ready;
    char message[BUFSIZ]; 
    int nb_read;

    // printf("[TABLE %d] on envoie la table !!!\n", num);
    send_packet(tube_send, table_to_packet(table), ip_vir);

    // signal(SIGALRM, process_killer);
    // alarm(TIME_TO_LIVE_MAX);

    FD_ZERO(&readfds);
    FD_SET(tube_recv, &readfds);

    for(int i = 0; i < NB_MODIF_TABLE_MAX; i++) {
        sleep(1);
        // printf("[router table] waiting block\n");
        timeout.tv_sec = TIME_READ_MAX;
        timeout.tv_usec = 0;
        ready = select(tube_recv + 1, &readfds, NULL, NULL, &timeout);
        
        if(ready > 0 && (nb_read = read(tube_recv, buff, SIZE_BUFF_BLOCK)) != 0) {
            if( nb_read == -1) {
                error_tab("[router_table] read tube from receiver error", EXIT_FAILURE, tube_send, tube_recv, table, buff);
            }
// printf("[router table] add block to table\n");
// printf("[table %d] add to table : %s\n", num, buff );

            if(table_add_block(table, buff)) {
// printf("[table %d] block added : %s\n",num,  buff);
                add_ip_vir_and_weight(buff, ip_vir);
                send_block(tube_send, buff);
            }
        } else {
// if(num == 3)table_display(table);
            export_table_router(table, num);
// table_display(table);
            sprintf(message,"[router_table %d] No more block to send", num);
            error_tab(message, EXIT_FAILURE, tube_send, tube_recv, table, buff);
        }
    }
    export_table_router(table, num);
// table_display(table);
    sprintf(message,"[router_table %d] number max operation reached", num);
    error_tab(message, EXIT_FAILURE, tube_send, tube_recv, table, buff);
}





/*------------------------------------------ ROUTER ------------------------------------------*/

char* create_path_import(int num) {
    char *path = malloc(sizeof(char)*BUFSIZ);
    sprintf(path, "../INPUT/config_router_%d.txt", num);
    return path;
}






void router_main(int num, char* ip_phy, int port_recv, int* port_tran, int nb_port, char* ip_vir) {
    int tube_send[2];
    int tube_recv[2];
    char *path_import;
    int pid_recv, pid_tab, pid_tran;
    Table table;
    int status;
    
    // printf("[router %d] creation path to config\n", num);
    path_import = create_path_import(num);

    // printf("[router %d] import table\n", num);
    if((table = table_import(path_import)) == NULL) {
        fprintf(stdout, "[router_main] Error loading config_router_%d\n", num);
        exit(EXIT_FAILURE);
    }

    // printf("[router %d] Creation pipe\n", num);
    if(pipe(tube_send) == -1 || pipe(tube_recv) == -1) {
        perror("[router_main] Error opening pipe");
        exit(EXIT_FAILURE);
    }

    

    // ######################################### FORK #########################################

    // printf("[router %d] Fork receiver\n", num);
    if ((pid_recv = fork()) == -1) {
        error_router("[router_main] fork receiver error",EXIT_FAILURE, tube_send, tube_recv, table);
        exit(1);
    } else if (pid_recv == 0){            close(tube_send[1]); close(tube_send[0]);
        close(tube_recv[0]);
        router_receiver(num, ip_phy, port_recv, tube_recv[1]);
        exit(1);
    }

    // printf("[router %d] Fork transmitter\n", num);
    if ((pid_tran = fork()) == -1) {
        error_router("[router_main] fork transmitter error",EXIT_FAILURE, tube_send, tube_recv, table);
        exit(1);
    } else if (pid_tran == 0){
            close(tube_recv[1]); close(tube_recv[0]);
            close(tube_send[1]);
            router_transmitter(num, ip_phy, port_tran, nb_port, tube_send[0]);
            exit(1);
    }

    // printf("[router %d] Fork table\n", num);
    if ((pid_tab = fork()) == -1) {
            error_router("[router_main] fork table error", EXIT_FAILURE,tube_send, tube_recv, table);
            exit(1);
    } else if (pid_tab == 0){
        close(tube_recv[1]);
        close(tube_send[0]);
        router_table(num,ip_vir, table, tube_send[1], tube_recv[0]);
        exit(1);
    }
    // printf("[router %d] sleep\n", num);


    // ######################################### END ROUTER #########################################

    close(tube_recv[0]); close(tube_recv[1]);
    close(tube_send[0]); close(tube_send[1]);

    printf("[router %d] start succesfully\n", num);
    sleep(TIME_TO_LIVE_MAX*2);
    free(path_import);

    kill(pid_recv, SIGTERM);
    kill(pid_tab, SIGTERM);
    kill(pid_tran, SIGTERM);

    
    waitpid(pid_recv, &status, 0);
    printf("[router %d] End process receiver ######################\n", num);
    waitpid(pid_tab, &status, 0);
    printf("[router %d] End process table ######################\n", num);
    waitpid(pid_tran, &status, 0);
    printf("[router %d] End process transmitter ######################\n", num);


}

