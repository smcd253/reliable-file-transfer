// server code for UDP socket programming
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
 
#define IP_PROTOCOL 0
#define PORT_NO 15050
#define BUFFER_SIZE 32
#define cipherKey 'S'
#define sendrecvflag 0
#define nofile "File Not Found!"
 
// funtion sending file
int sendFile(FILE* fp, char* buf, int s)
{
    int i, len;
    if (fp == NULL) {
        strcpy(buf, nofile);
        len = strlen(nofile);
        buf[len] = EOF;
        // for (i = 0; i <= len; i++)
        //     buf[i] = Cipher(buf[i]);
        return 1;
    }
 
    char ch;
    for (i = 0; i < s; i++) {
        ch = fgetc(fp);
        //ch2 = Cipher(ch);
        buf[i] = ch;
        if (ch == EOF)
            return 1;
    }
    return 0;
}
 
// driver code
int main()
{
    int sockfd, nBytes;
    struct sockaddr_in addr_con;
    int addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT_NO);
    addr_con.sin_addr.s_addr = INADDR_ANY;
    char buffer[BUFFER_SIZE];
    FILE* fp;
 
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
 
    while (1) {
        printf("\nWaiting for file name...\n");
 
        // receive file name
        bzero(buffer, BUFFER_SIZE);

        nBytes = recvfrom(sockfd, buffer,
                          BUFFER_SIZE, sendrecvflag,
                          (struct sockaddr*)&addr_con, &addrlen);
 
        fp = fopen(buffer, "r");
        printf("\nFile Name Received: %s\n", buffer);
        if (fp == NULL)
            printf("\nFile open failed!\n");
        else
            printf("\nFile Successfully opened!\n");
 
        while (1) {
 
            // process
            if (sendFile(fp, buffer, BUFFER_SIZE)) {
                sendto(sockfd, buffer, BUFFER_SIZE,
                       sendrecvflag, 
                    (struct sockaddr*)&addr_con, addrlen);
                break;
            }
 
            // send
            sendto(sockfd, buffer, BUFFER_SIZE,
                   sendrecvflag,
                (struct sockaddr*)&addr_con, addrlen);
            bzero(buffer, BUFFER_SIZE);
        }
        if (fp != NULL)
            fclose(fp);
    }
    return 0;
}

