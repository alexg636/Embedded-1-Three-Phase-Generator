#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>

#define LOCAL_IP "127.0.0.1"
#define LOCAL_PORT 5367

#define BUFF_SIZE 32

// Function to generate random noise on a 1s, 10s, 100s, 1000s level
double random_noise(double level) {
    srand(time(0));
    return (random() % 1000/(1000.0/level));
}

// Function converting degrees to radians.
double to_radians(double degrees) {
    return M_PI*(degrees/180.0);
}

// Function convert a double data type into a byte array
void double2byte(unsigned char temp_bytes[8], double value) {
    union {
        double num;
        unsigned char bytes[8];
    } unionized;

    unionized.num = value;
    memcpy(temp_bytes, unionized.bytes, 8);
}

// Function will concatenate 4x 8 byte char arrays.
static unsigned char concatenate(unsigned char arr1[], unsigned char arr2[], unsigned char arr3[], unsigned char arr4[], unsigned char conarray[]) {
    memcpy(conarray + 0, arr1, sizeof(arr1));
    memcpy(conarray + 8, arr2, sizeof(arr2));
    memcpy(conarray + 16, arr3, sizeof(arr3));
    memcpy(conarray + 24, arr4, sizeof(arr4));

    return conarray;
}

// Function generating three phase outputs as array and forms UDP payload.
static unsigned char * threephase(double degrees, double noise_level) {
    printf("Ideal Degrees: %f\n", degrees);
    double p1 = to_radians(degrees + random_noise(noise_level));
    printf("Phase 1 Degrees: %f\n", (degrees + random_noise(noise_level)));
    double p2 = to_radians(degrees + 120.0 + random_noise(noise_level));
    printf("Phase 2 Degrees: %f\n", (degrees + 120.0 + random_noise(noise_level)));
    double p3 = to_radians(degrees + 240.0 + random_noise(noise_level));
    printf("Phase 3 Degrees: %f\n\n", (degrees + 240.0 + random_noise(noise_level)));


    // C does not return the address of a local var outside function, therefore use static variable.
    static unsigned char payload[32];
    unsigned char degree_arr[8];
    unsigned char phase_1[8];
    unsigned char phase_2[8];
    unsigned char phase_3[8];
    double2byte(degree_arr, degrees);
    double2byte(phase_1, 120.0*sin(p1));
    double2byte(phase_2, 120.0*sin(p2));
    double2byte(phase_3, 120.0* sin(p3));

    concatenate(degree_arr, phase_1, phase_2, phase_3, payload);
    return payload;
}

int main(int argc, char* argv[]) {
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
        // No argument: return "noiseless sinusoids"
        unsigned char *threephase_arr;
        if (argc == 1) {
            threephase_arr = threephase(deg, 0.00001);
        }
        else if (argc > 1) {
            threephase_arr = threephase(deg, strtod(argv[1], NULL));
        }
        
        // unsigned char *threephase_arr = threephase(deg);
        int status = sendto(sockfd, threephase_arr, BUFF_SIZE, 0, (const struct sockaddr *)&clientaddr, sizeof(clientaddr));
        // Label which degree/radian currently executing.
        double rad = to_radians(deg);
        // printf("Degrees: %f -- Radians: %f\n", deg, rad);

        // int i;
        // printf("\nDegree is:");
        // for (i=0;i<8;i++) {
        //     printf ("%02x", threephase_arr[i]);
        // }
        // printf("\nPhase 1 is:");
        // for (i=8;i<16;i++) {
        //     printf ("%02x", threephase_arr[i]);
        // }
        // printf("\nPhase 2 is:");
        // for (i=16;i<24;i++) {
        //     printf ("%02x", threephase_arr[i]);
        // }
        // printf("\nPhase 3 is:");
        // for (i=24;i<32;i++) {
        //     printf ("%02x", threephase_arr[i]);
        // }
        // printf("\n\n");
        deg += 1.0;
        usleep(500000);
    }
}