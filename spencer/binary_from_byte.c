#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define ACK_SIZE 20000

struct ack_packet{
	uint8_t type;
	uint8_t packet_tracker[ACK_SIZE];
};

int main(int argc, char *argv[])
{
    struct ack_packet ack_packet1;

    for (int i = 0; i < ACK_SIZE; i++)
    {
        uint8_t randNum = (uint8_t)(rand() % 256);
        memcpy(&ack_packet1.packet_tracker[i], &randNum, 8);
        printf("ack_packet1.packet_tracket[%d] = %d\n", i, ack_packet1.packet_tracker[i]);
        printf("Bit Sequence for packet_tracker[%d]: ", i);
        for (int j = 0; j < 8; j++)
            printf("%d", (ack_packet1.packet_tracker[i] >> j) & 0x01);
        printf("\n");
    }
}