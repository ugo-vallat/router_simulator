#include "router.h"
#include <stdio.h>
#include <stdlib.h>




int main(void) {
    int *port1 = malloc(sizeof(int)*1);
    int *port2 = malloc(sizeof(int)*2);
    int *port3 = malloc(sizeof(int)*1);

    port1[0] = 20000;
    port2[0] = 10000;
    port2[1] = 30000;
    port3[0] = 20000;
    switch (fork()) {
        case -1:
            perror("[main test_router] error fork");
            exit(1);
        case 0:
            router_main(1, "127.0.0.1", 10000, port1, 1, "1.1.1.1");
            exit(0);
            break;
        default:
            // router_main(2, "127.0.0.1", 20000, port2, 1);
            break;
    }

    switch(fork()) {
        case -1:
            perror("[main test_router] error fork");
            exit(1);
        case 0:
            router_main(3, "127.0.0.1", 30000, port3, 1, "3.3.3.3");
            exit(0);
            break;
        default:
            router_main(2, "127.0.0.1", 20000, port2, 2, "2.2.2.2");
            break;
    }

    free(port1);
    free(port2);
    free(port3);
    
    

    return 0;
}