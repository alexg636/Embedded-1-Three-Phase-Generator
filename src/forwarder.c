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
#define FOREIGN_IP "192.168.1.14"

#define BUFF_SIZE 4
double buffer[BUFF_SIZE];

void buffer_extraction(void) {
    printf("Degrees: %f\n", *(buffer));
    for (int i=1; i<4; i++) {
        printf("Phase %d: %f\n", i, buffer[i]);
    }
    printf("\n");
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in remaddr;
    socklen_t addrlen = sizeof(remaddr);
    struct sockaddr_in clientaddr;

    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
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
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        exit(-1);
    }

    while (1) {
    int size = recvfrom(sockfd, (char *)buffer, BUFF_SIZE, 0, (struct sockaddr *)&remaddr, sizeof(addrlen));
    printf("Size received: %d\n", size);
    buffer_extraction();

    }
}