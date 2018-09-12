// source: https://www.geeksforgeeks.org/c-program-for-file-transfer-using-udp/

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
#define sendrecvflag 0
 
#define FILENAMELENGTH 30
#define data_size 1400
#define UDP_BURST 5
struct Init_Packet{
    u_int file_size;
    u_int rcv_packet_size;
};
struct Data_Packet{
    u_int sequence_number;
    unsigned char data[data_size];
};
 
// driver code
int main()
{
    int sockfd, nBytes;
    struct sockaddr_in addr_con;
    socklen_t addrlen = sizeof(addr_con);
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT_NO);
    addr_con.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    // buff stuff
    int init_packet_size = sizeof(struct Init_Packet);
    int data_packet_size = sizeof(struct Data_Packet);
    char init_buffer[init_packet_size];
    char buffer[data_packet_size];
    FILE* oFile;
 
    // socket()
    sockfd = socket(AF_INET, SOCK_DGRAM, IP_PROTOCOL);
 
    if (sockfd < 0)
        printf("\nsocket file descriptor not received!!\n");
    else
        printf("\nsocket file descriptor %d received\n", sockfd);
    
    // grab init packet to get num packets, packet size, and last packet size
    while (nBytes != init_packet_size)
    {
        printf("am I here?\n");
        bzero(init_buffer, init_packet_size);
        printf("here?\n");
        nBytes = recvfrom(sockfd, init_buffer, init_packet_size, sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
        printf("stuck in while\n");
    }
    struct Init_Packet* iPacket = (struct Init_Packet*)init_buffer;
    printf("iPacket->file_size = %d", iPacket->file_size);
    printf("iPacket->rcv_packet_size = %d", iPacket->rcv_packet_size);

    // send init packet receive ack
    char init_ack_buf[sizeof(char)] = "a";
    for (int i = 0; i < UDP_BURST; i++)
    {
        sendto(sockfd, init_ack_buf, sizeof(char), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);        
    }

    // send something to server
    // sendto(sockfd, fileNameBuf, BUFFER_SIZE, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);


    // // init_packet = init_buffer;
    // int w_count = 0;
    // for(w_count = 0; w_count < chunks - 1 ;w_count++)
    // {
    //     fwrite (buffer[w_count] , sizeof(char), data_size, oFile);
    // }
    // struct packet* recv_packet = (struct packet*)packet_tobe_sent; 
    // printf ("recv_packet chunk no  : %d\n", recv_packet->sequence_number);
    // fwrite (recv_packet->data, sizeof(char), final_chunk, oFile);
    // /* the whole file is now loaded in the memory buffer. */

    return 0;
}