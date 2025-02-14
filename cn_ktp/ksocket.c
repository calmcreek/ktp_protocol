#include "ksocket.h"

// Shared memory for KTP sockets
KTPSocket *shared_mem = NULL;
int shmid = -1;

// Initialize shared memory
void init_shared_memory() {
    printf("DEBUG: Initializing shared memory...\n");
    key_t key = ftok("ksocket", 65);
    shmid = shmget(key, MAX_KTP_SOCKETS * sizeof(KTPSocket), 0666 | IPC_CREAT);
    shared_mem = (KTPSocket *)shmat(shmid, NULL, 0);
    if (shared_mem == (void *)-1) {
        perror("shmat failed");
        exit(EXIT_FAILURE);
    }
    printf("DEBUG: Shared memory initialized successfully.\n");
}

// Create KTP socket
int k_socket() {
    printf("DEBUG: Creating KTP socket...\n");
    if (shared_mem == NULL) init_shared_memory();

    for (int i = 0; i < MAX_KTP_SOCKETS; i++) {
        if (!shared_mem[i].in_use) {
            shared_mem[i].in_use = 1;
            shared_mem[i].owner_pid = getpid();
            shared_mem[i].udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
            shared_mem[i].send_count = shared_mem[i].recv_count = 0;
            printf("DEBUG: KTP socket %d created.\n", i);
            return i;
        }
    }
    printf("DEBUG: No available KTP sockets.\n");
    errno = ENOSPACE;
    return -1;
}

// Bind KTP socket
int k_bind(int ktp_sock, struct sockaddr_in *local, struct sockaddr_in *remote) {
    printf("DEBUG: Binding socket %d...\n", ktp_sock);
    if (shared_mem == NULL) init_shared_memory();

    if (ktp_sock < 0 || ktp_sock >= MAX_KTP_SOCKETS || !shared_mem[ktp_sock].in_use) {
        errno = ENOTBOUND;
        printf("DEBUG: Bind failed! Invalid socket ID.\n");
        return -1;
    }

    shared_mem[ktp_sock].local_addr = *local;
    shared_mem[ktp_sock].remote_addr = *remote;
    int result = bind(shared_mem[ktp_sock].udp_socket, (struct sockaddr *)local, sizeof(*local));
    if (result == -1) perror("bind failed");
    printf("DEBUG: Bind successful for socket %d.\n", ktp_sock);
    return result;
}

// Send message
int k_sendto(int ktp_sock, const char *msg, int len) {
    printf("DEBUG: Sending message from socket %d...\n", ktp_sock);
    if (shared_mem == NULL) init_shared_memory();

    if (ktp_sock < 0 || !shared_mem[ktp_sock].in_use) {
        errno = ENOTBOUND;
        printf("DEBUG: Send failed! Socket not in use.\n");
        return -1;
    }

    if (shared_mem[ktp_sock].send_count >= MAX_MSGS) {
        errno = ENOSPACE;
        printf("DEBUG: Send failed! Send buffer full.\n");
        return -1;
    }

    strcpy(shared_mem[ktp_sock].send_buffer[shared_mem[ktp_sock].send_count].data, msg);
    shared_mem[ktp_sock].send_count++;

    int sent_bytes = sendto(
        shared_mem[ktp_sock].udp_socket, msg, len, 0,
        (struct sockaddr *)&shared_mem[ktp_sock].remote_addr, sizeof(struct sockaddr_in));

    if (sent_bytes == -1) perror("sendto failed");
    printf("DEBUG: Message sent, bytes = %d\n", sent_bytes);
    return sent_bytes;
}

// Receive message
// Receive message
int k_recvfrom(int ktp_sock, char *buf, int len) {
    struct sockaddr_in sender_addr;
    socklen_t addr_len = sizeof(sender_addr);

    if (shared_mem == NULL) init_shared_memory();
    
    if (ktp_sock < 0 || !shared_mem[ktp_sock].in_use) {
        errno = ENOMESSAGE;
        return -1;
    }

    printf("DEBUG: Waiting for messages on socket %d...\n", ktp_sock);

    // **Wait until we receive a message from the UDP socket**
    int bytes = recvfrom(shared_mem[ktp_sock].udp_socket, buf, len, 0,
                         (struct sockaddr *)&sender_addr, &addr_len);
    
    if (bytes < 0) {
        perror("DEBUG: recvfrom failed");
        return -1;
    }

    buf[bytes] = '\0';  // Null-terminate received string
    printf("DEBUG: Received %d bytes: %s\n", bytes, buf);

    return bytes;
}




// Close socket
int k_close(int ktp_sock) {
    printf("DEBUG: Closing socket %d...\n", ktp_sock);
    if (shared_mem == NULL) init_shared_memory();

    if (ktp_sock < 0 || !shared_mem[ktp_sock].in_use) {
        printf("DEBUG: Close failed! Invalid socket.\n");
        return -1;
    }

    close(shared_mem[ktp_sock].udp_socket);
    shared_mem[ktp_sock].in_use = 0;
    printf("DEBUG: Socket %d closed successfully.\n", ktp_sock);
    return 0;
}

// Drop message simulation
int dropMessage(float p) {
    int drop = ((float)rand() / RAND_MAX) < p;
    printf("DEBUG: dropMessage() -> %d (p = %.2f)\n", drop, p);
    return drop;
}

