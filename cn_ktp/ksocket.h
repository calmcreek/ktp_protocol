#ifndef KSOCKET_H
#define KSOCKET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

#define SOCK_KTP 100    // Custom socket type for KTP
#define T 5             // Timeout value in seconds
#define P 0.1           // Packet loss probability

#define MAX_KTP_SOCKETS 10  // Max KTP sockets allowed
#define BUFFER_SIZE 1024     // Message buffer size
#define MAX_MSGS 50          // Max messages in buffer

// Error codes
#define ENOSPACE 1001    // No space available
#define ENOTBOUND 1002   // Socket not bound
#define ENOMESSAGE 1003  // No message available

// Message structure
typedef struct {
    int seq_num;
    char data[BUFFER_SIZE];
} KTPMessage;

// Send and Receive Window structures
typedef struct {
    int size;
    int messages[MAX_MSGS];
} Window;

// KTP Socket structure
typedef struct {
    int in_use;
    pid_t owner_pid;
    int udp_socket;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    KTPMessage send_buffer[MAX_MSGS];
    KTPMessage recv_buffer[MAX_MSGS];
    int send_count, recv_count;
    Window swnd;
    Window rwnd;
} KTPSocket;

// Function prototypes
int k_socket();
int k_bind(int ktp_sock, struct sockaddr_in *local, struct sockaddr_in *remote);
int k_sendto(int ktp_sock, const char *msg, int len);
int k_recvfrom(int ktp_sock, char *buf, int len);
int k_close(int ktp_sock);
void init_shared_memory();
int dropMessage(float p);

#endif

