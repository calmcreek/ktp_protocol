#include "ksocket.h"

int main() {
    init_shared_memory();
    printf("KTP socket system initialized.\n");
    while (1) sleep(10);
}

