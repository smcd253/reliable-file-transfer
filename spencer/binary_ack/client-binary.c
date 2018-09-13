// source :https://www.geeksforgeeks.org/c-program-for-file-transfer-using-udp/

// client code for UDP socket programming
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
#define IP_ADDRESS "127.0.0.1" // localhost
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
    addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);

 
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
 
    if (sockfd < 0)
        printf("\nsocket file descriptor not received!!\n");
    else
        printf("\nsocket file descriptor %d received\n", sockfd);
    
    // build all data packets with sequence numbers and append to buffer
    struct Data_Packet* packetBuffer;
    packetBuffer = malloc(sizeof(struct Data_Packet) * BUFFER_SIZE);
    char dataBuffer[BUFFER_SIZE][sizeof(struct Data_Packet)];
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        // put into data packet struct buffer
        snprintf(packetBuffer[i].data, PACKET_SIZE, "%d-packet", i);
        printf("%s", packetBuffer[i].data);
        packetBuffer[i].sequenceNumber = i;

        // throw into dataBuffer
        memset(dataBuffer[i],0,sizeof(struct Data_Packet));
        memcpy(dataBuffer[i],(const unsigned char*)&packetBuffer[i],sizeof(packetBuffer[i]));
    }
    // build buffer
    bool* buffer = (bool*) malloc (BUFFER_SIZE*sizeof(bool));
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        buffer[i] = false;
    }

    // send binary ack buffer to client
    sendto(sockfd, buffer, BUFFER_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);

    while (1) 
    {   
        // see if ack
        bool cumAck = true;
        printf("buffer = [");
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (i < BUFFER_SIZE - 1)
                printf("%d, ", buffer[i]);
            else printf("%d]\n", buffer[i]);
            cumAck &= buffer[i];
        }

        if(cumAck)
        {
            printf("cumAck!");
            break;
        }

        // send missing packets
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if(!buffer[i])
            {
                // printf("Sending %s\n", dataBuffer[i]);
                // sendto(sockfd, dataBuffer[i], BUFFER_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);

                printf("Sending %s\n", packetBuffer[i].data);
                sendto(sockfd, dataBuffer[i], BUFFER_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
            }
        }

        // clear buffer for receipt of client response
        bzero(buffer, BUFFER_SIZE);
        // wait to recieve server's list
        nBytes = recvfrom(sockfd, buffer, BUFFER_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
    }
    return 0;
}