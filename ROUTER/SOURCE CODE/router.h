#ifndef __ROUTER_H__
#define __ROUTER_H__

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


void router_main(int num, char* ip, int port_recv, int* port_tran, int nb_port, char* ip_vir);



#endif