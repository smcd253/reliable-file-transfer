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
#include <math.h>
 
#define IP_PROTOCOL 0
#define PORT_NO 15050
#define sendrecvflag 0
#define nofile "File Not Found!"
 
#define FILENAMELENGTH 30
#define data_size 1400
#define packet_size 1404

struct Init_Packet{
    u_int file_size;
    u_int send_packet_size;
};
struct packet{
    u_int sequence_number;
    unsigned char data[data_size];
};

int main()
{   
    // read file and build DRAM matrix
    FILE * pFile;
    long lSize;

    size_t result;
    int chunks;
    int final_chunk;
    struct packet data_packet = {0};
    unsigned char * packet_tobe_sent;

    pFile = fopen ( "test.mov" , "rb" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

    // obtain file size:
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);
    printf("File Size: %ld\n", lSize);
    chunks = ceil(lSize/(float)data_size);

    // allocate memory to contain the whole file:
    char **buffer = (char**) malloc (chunks*sizeof(char*));
    int count = 0;

    /// reading from file
    for(count = 0; count < chunks - 1;count++)
    {
        buffer[count] = (char*) malloc (data_size*sizeof(char));
        result = fread (buffer[count],1,data_size,pFile);
    } 
    printf("No of chunks : %d\n", chunks);
    printf("Max file size : %ld\n", lSize);
    printf("Memory read : %d\n", (chunks - 1)*data_size);
    printf("Memory left to read : %ld\n", lSize - (chunks-1)*data_size);
    printf("count value  : %d\n", count);

    /// remaining memory chunk lSize - chunks*data_size
    final_chunk = lSize - (chunks-1)*data_size;

    buffer[count] = (char*) malloc (final_chunk*sizeof(char));
    result = fread (buffer[count],1,final_chunk,pFile);
    printf("File Size read: %ld, count : %d\n", result,count);

    // connection definitions
    int sockfd, nBytes;
    struct sockaddr_in addr_con;
    socklen_t addrlen = sizeof(addr_con);
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

    // build init_packet struct
    struct Init_Packet init_packet;
    init_packet.file_size = lSize;
    init_packet.send_packet_size = packet_size;
    
    // build init_packet datagram buffer
    packet_tobe_sent = (unsigned char*)malloc(sizeof(struct Init_Packet));
    memset(packet_tobe_sent,0,sizeof(struct Init_Packet));
    memcpy(packet_tobe_sent,(const unsigned char*)&init_packet,sizeof(init_packet));
    
    // send init_packet datagram and wait for ack
    char init_ack_buf[sizeof(char)];
    char ack_compare[sizeof(char)] = "a";
    
    while (strcmp(init_ack_buf, ack_compare) != 0)
    {
        sendto(sockfd, packet_tobe_sent, sizeof(init_packet), sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
        recvfrom(sockfd, init_ack_buf, sizeof(char), sendrecvflag, (struct sockaddr*)&addr_con, &addrlen);
        bzero(init_ack_buf, sizeof(char));
    }

    
    // // send out datagrams from each index of buffer
    // for (int i = 0; i < chunks - 1; i++)
    // {
    //     // build packet struct
    //     data_packet.sequence_number = i;
    //     memcpy(data_packet.data,buffer[i], data_size);

    //     packet_tobe_sent = (unsigned char*)malloc(sizeof(struct packet));
    //     memset(packet_tobe_sent,0,sizeof(struct packet));
    //     memcpy(packet_tobe_sent,(const unsigned char*)&data_packet,sizeof(data_packet));

    //     sendto(sockfd, packet_tobe_sent, packet_size, sendrecvflag, (struct sockaddr*)&addr_con, addrlen);
    //     bzero(packet_tobe_sent, packet_size);
    // }
    fclose(pFile);
    return 0;
}

