// source: https://www.geeksforgeeks.org/c-program-for-file-transfer-using-udp/

// server code for UDP socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#define IP_PROTOCOL 0
#define PORT_NO 15050
#define sendrecvflag 0
#define BUFFER_SIZE 16
#define PACKET_SIZE 16
 
struct Data_Packet
{
    int sequenceNumber;
    char data[PACKET_SIZE];
};

// driver code
int main()
{
    int sockfd, nBytes;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT_NO);
    addr_con.sin_addr.s_addr = INADDR_ANY;
 
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
 
    if (sockfd < 0)
        printf("\nfile descriptor not received!!\n");
    else
        printf("\nfile descriptor %d received\n", sockfd);
 
    // bind()
    if (bind(sockfd, (struct sockaddr*)&addr_con, sizeof(addr_con)) == 0)
        printf("\nSuccessfully binded!\n");
    else
        printf("\nBinding Failed!\n");

    // receieved data buffer
    char **receivedDataBuffer = (char**) malloc (BUFFER_SIZE*sizeof(char*));
    char* dataBuffer = (char*) malloc (BUFFER_SIZE*sizeof(struct Data_Packet));

    // binary buffer
    bool* binaryBuffer = (bool*) malloc (BUFFER_SIZE*sizeof(bool));

     // receive binary buffer to establish connection
    bzero(binaryBuffer, BUFFER_SIZE);
    nBytes = recvfrom(sockfd, binaryBuffer, BUFFER_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);


    // struct array to throw interpret incoming packets
    struct Data_Packet* packetArray;
    packetArray = malloc(sizeof(struct Data_Packet) * BUFFER_SIZE);

    while (1) 
    {
        // print
        printf("binaryBuffer = [");
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (i < BUFFER_SIZE - 1)
                printf("%d, ", binaryBuffer[i]);
            else printf("%d]\n", binaryBuffer[i]);
        }

        // receive data
        int receiveCount = 0;
        while (receiveCount < BUFFER_SIZE)
        {
            nBytes = recvfrom(sockfd, dataBuffer, BUFFER_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
            
            // create new Data_Packet struct from incoming buffer
            struct Data_Packet* receivedPacket = (struct Data_Packet*)dataBuffer;
            
            printf("received packet: %s\n", receivedPacket->data);

            // put packet in proper location in receive buffer
            printf("packetNum = %d\n", receivedPacket->sequenceNumber);

            receivedDataBuffer[receivedPacket->sequenceNumber] = receivedPacket->data;
            bzero(dataBuffer, BUFFER_SIZE);

            // ack the corresponding index in the bool array
            binaryBuffer[receivedPacket->sequenceNumber] = true;
            printf("boolean in transit: %d\n ", binaryBuffer[receivedPacket->sequenceNumber]);

            receiveCount++;
            printf("receiveCount cannot be added?\n");
        }
        // print
        printf("before send binaryBuffer = [");
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (i < BUFFER_SIZE - 1)
                printf("%d, ", binaryBuffer[i]);
            else printf("%d]\n", binaryBuffer[i]);
        }
        // send updated buffer back
        sendto(sockfd, binaryBuffer, BUFFER_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    }

    return 0;
}

