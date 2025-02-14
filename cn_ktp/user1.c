#include "ksocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    struct sockaddr_in local, remote;
    int sock;
    char *msg = "Hello, KTP!";
    
    // Default values
    char *local_ip = "127.0.0.1";
    int local_port = 8080;
    char *remote_ip = "127.0.0.1";
    int remote_port = 9090;

    // Use command-line arguments if provided
    if (argc == 5) {
        local_ip = argv[1];
        local_port = atoi(argv[2]);
        remote_ip = argv[3];
        remote_port = atoi(argv[4]);
    }

    sock = k_socket();

    local.sin_family = AF_INET;
    local.sin_port = htons(local_port);
    local.sin_addr.s_addr = inet_addr(local_ip);

    remote.sin_family = AF_INET;
    remote.sin_port = htons(remote_port);
    remote.sin_addr.s_addr = inet_addr(remote_ip);

    k_bind(sock, &local, &remote);
    k_sendto(sock, msg, strlen(msg) + 1);
    k_close(sock);

    return 0;
}

