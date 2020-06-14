#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define LOCAL_IP "127.0.0.1"
#define LOCAL_PORT 5367

#define BUFF_SIZE 4

double buffer[BUFF_SIZE];

// Function converting degrees to radians.
double to_radians(double degrees) {
    return M_PI*(degrees/180.0);
}

// Function generating three phase outputs as array.
double * threephase(double degrees) {
    double p1 = to_radians(degrees);
    double p2 = to_radians(degrees + 120.0);
    double p3 = to_radians(degrees + 240.0);

    // C does not return the address of a local var outside function, therefore use static variable.
    static double array[3];
    array[0] = 120*sin(p1);
    array[1] = 120*sin(p2);
    array[2] = 120*sin(p3);
    return array;
}

// Function generating UDP payload
double UDP_payload(double degrees) {
    double *threephase_arr = threephase(degrees);
    buffer[0] = degrees;
    buffer[1] = *(threephase_arr);
    buffer[2] = *(threephase_arr + 1);
    buffer[3] = *(threephase_arr + 2);
}

int main(void) {
    double deg = 0.0;
    int sockfd;
    struct sockaddr_in clientaddr;

    // Create socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        exit(-1);
    }

    memset(&clientaddr, 0, sizeof(clientaddr));

    clientaddr.sin_family = AF_INET;
    clientaddr.sin_port = htons(LOCAL_PORT);
    clientaddr.sin_addr.s_addr = inet_addr(LOCAL_IP);

    while (1) {
        sendto(sockfd, buffer, BUFF_SIZE, 0, (const struct sockaddr *)&clientaddr, sizeof(clientaddr));
        // Label which degree/radian currently executing.
        double rad = to_radians(deg);
        printf("Degrees: %f -- Radians: %f\n", deg, rad);

        double *threephase_arr = threephase(deg);
        for (int i=0; i<3; i++) {
            printf("Phase %d: %f\n", i+1, *(threephase_arr+i));
        }
        printf("\n");
        deg += 1.0;
        usleep(5000);
    }
}