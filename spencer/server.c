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
 
#define IP_PROTOCOL 0
#define PORT_NO 15050
#define BUFFER_SIZE 32
#define sendrecvflag 0
#define nofile "File Not Found!"
 

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
    // --------------  in real code you would check for errors in malloc here ----------------------
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

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
 
        fp = fopen(buffer, "rb");
        printf("\nFile Name Received: %s\n", buffer);
        if (fp == NULL)
            printf("\nFile open failed!\n");
        else
            printf("\nFile Successfully opened!\n");
 
        while (fread(buffer, sizeof(buffer), BUFFER_SIZE, fp) == BUFFER_SIZE) {
            // debug
            printf("Buffer Debug:\n");
            for (int i = 0; i < strlen(buffer); i++)
                printf(buffer[i]);                
            printf("\n");
            // send
            sendto(sockfd, buffer, BUFFER_SIZE,
                   sendrecvflag,
                (struct sockaddr*)&addr_con, addrlen);
            bzero(buffer, BUFFER_SIZE);
        }
        if (fp != NULL)
            fclose(fp);

        // char rcvBufAppend[9] = "received-";
        // FILE* receivedFile = fopen(concat(rcvBufAppend, "hello.txt"), "wb");
        // // debug
        // while (fwrite(buffer, sizeof(char), strlen(buffer), receivedFile) > 0) 
        // {
        //     // receive packet
        //     bzero(buffer, BUFFER_SIZE);
        //     nBytes = recvfrom(sockfd, buffer, BUFFER_SIZE,
        //                     sendrecvflag, (struct sockaddr*)&addr_con,
        //                     &addrlen);
        // }
    }
    return 0;
}

