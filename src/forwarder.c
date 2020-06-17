#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define RECV_PORT 5367
#define SEND_PORT 6367
#define LOCAL_IP "127.0.0.1"
#define FOREIGN_IP "192.168.1.13"

#define BUFF_SIZE 32

void buffer_extraction(unsigned char buffer[]) {
    int i;
    printf("\nDegree is:");
    for (i=0;i<8;i++) {
        printf ("%02x", buffer[i]);
    }
    printf("\nPhase 1 is:");
    for (i=8;i<16;i++) {
        printf ("%02x", buffer[i]);
    }
    printf("\nPhase 2 is:");
    for (i=16;i<24;i++) {
        printf ("%02x", buffer[i]);
    }
    printf("\nPhase 3 is:");
    for (i=24;i<32;i++) {
        printf ("%02x", buffer[i]);
    }
    printf("\n\n");
}

int main() {
    int sock_server;
    int sock_client;

    struct sockaddr_in servaddr;
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    struct sockaddr_in clientaddr;

    // Create socket file descriptor for server
    if ((sock_server = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        exit(-1);
    }

    // Create socket file descriptor for client
    if ((sock_client = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        exit(-1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&clientaddr, 0, sizeof(clientaddr));

    // Server setup
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(RECV_PORT);
    servaddr.sin_addr.s_addr = inet_addr(LOCAL_IP);

    // Client setup
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(SEND_PORT);
    clientaddr.sin_addr.s_addr = inet_addr(FOREIGN_IP);
    
    // Bind to server
    if (bind(sock_server, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        exit(-1);
    }

    while (1) {
        unsigned char *buffer[BUFF_SIZE];
        int size = recvfrom(sock_server, buffer, BUFF_SIZE, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        printf("Size received: %d\n", size);
        buffer_extraction(buffer);

        int status = sendto(sock_client, buffer, BUFF_SIZE, 0, (const struct sockaddr *)&clientaddr, sizeof(clientaddr));

    }
}