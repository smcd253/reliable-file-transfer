#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
 
#define FILENAMELENGTH 30
#define data_size 60000
#define packet_size 60004

struct packet{
    u_int sequence_number;
    unsigned char data[data_size];
};
 
int main()
{
    // initialization 

    FILE * pFile;
    FILE * oFile;
    long lSize;

    char data[60000];
    size_t result;
    int chunks;
    int final_chunk;
    struct packet data_packet = {0};
    char * packet_tobe_sent;

    pFile = fopen ( "test.mov" , "rb" );
    oFile = fopen ( "test-received.mov" , "wb" );
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


    data_packet.sequence_number = count;
    memcpy(data_packet.data,buffer[count],data_size);

    packet_tobe_sent = (unsigned char*)malloc(sizeof(struct packet));
    memset(packet_tobe_sent,0,sizeof(struct packet));
    memcpy(packet_tobe_sent,(const unsigned char*)&data_packet,sizeof(data_packet));

    

    int w_count = 0;
    for(w_count = 0; w_count<chunks-1;w_count++)
    {
        fwrite (buffer[w_count] , sizeof(char), data_size, oFile);
    }
    struct packet* recv_packet = (struct packet*)packet_tobe_sent; 
    printf("recv_packet chunk no  : %d\n", recv_packet->sequence_number);
    fwrite (recv_packet->data, sizeof(char), final_chunk, oFile);
    /* the whole file is now loaded in the memory buffer. */

    // terminate
    fclose (pFile);
    fclose (oFile);
    free (buffer);
    return 0;
}