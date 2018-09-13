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
 
#define IP_PROTOCOL 0
#define IP_ADDRESS "127.0.0.1" // localhost
#define PORT_NO 15050
#define BUFFER_SIZE 32
#define cipherKey 'S'
#define sendrecvflag 0
 
// driver code
int main()
{
    int sockfd, nBytes;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT_NO);
    addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    char buffer[BUFFER_SIZE];
    char fileNameBuf[BUFFER_SIZE];
    FILE* fp;
 
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM,
                    IP_PROTOCOL);
 
    if (sockfd < 0)
        printf("\nsocket file descriptor not received!!\n");
    else
        printf("\nsocket file descriptor %d received\n", sockfd);
    

    while (1) {
        printf("\nPlease enter file name to receive:\n");
        scanf("%s", fileNameBuf);
        sendto(sockfd, fileNameBuf, BUFFER_SIZE,
               sendrecvflag, (struct sockaddr*)&addr_con,
               addrlen);

        // create file to write received buffer to
        char rcvBufAppend[9] = "received-";
        FILE* receivedFile = fopen(concat(rcvBufAppend, fileNameBuf), "w");
        if (receivedFile == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }
        printf("\n---------Data Received---------\n");
 
        while (1) {
            // receive
            bzero(buffer, BUFFER_SIZE);
            nBytes = recvfrom(sockfd, buffer, BUFFER_SIZE,
                              sendrecvflag, (struct sockaddr*)&addr_con,
                              &addrlen);
 
            // process
            if (recvFile(receivedFile, buffer, BUFFER_SIZE)) {
                break;
            }
        }
        if(receivedFile != NULL)
            fclose(receivedFile);
        printf("\n-------------------------------\n");
    }
    return 0;
}