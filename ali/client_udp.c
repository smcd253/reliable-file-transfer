/* UDP client in the internet domain */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
 
#define data_size 60000

struct packet{
	unsigned char type;
	int sequence_number;
	unsigned char data[data_size];
};

struct Init_PACKET{
	unsigned char type;
	u_int file_size;
	u_int chunk_size;
};

struct ack_packet{
	unsigned char type;
	bool * packet_received;
};

void error(const char *);

int main(int argc, char *argv[])
{
   // file initializationa
	FILE * pFile;
	long lSize;

	char data[60000];
	size_t result;
	int chunks;
	int final_chunk;
	struct packet data_packet = {0};
	char * packet_tobe_sent;
	char buffer1[256];

	// control init
	struct Init_PACKET init_packet = {0};
	char * send_buffer;

	// socket initializations
	int sock, n;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;

	// soclet configuration
	if (argc != 3) { printf("Usage: server port\n");
	              exit(1);
	}

	sock= socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) error("socket");

	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if (hp==0) error("Unknown host");

	bcopy((char *)hp->h_addr, (char *)&server.sin_addr,hp->h_length);
	server.sin_port = htons(atoi(argv[2]));
	length=sizeof(struct sockaddr_in);

  //opening up a file 

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
	printf("No of shunks : %d\n", chunks);
	printf("Max file size : %ld\n", lSize);
	printf("Memory read : %d\n", (chunks - 1)*data_size);
	printf("Memory left to read : %ld\n", lSize - (chunks-1)*data_size);
	printf("count value  : %d\n", count);


	/// remaining memory chunk lSize - chunks*data_size
	final_chunk = lSize - (chunks-1)*data_size;

	buffer[count] = (char*) malloc (final_chunk*sizeof(char));
	result = fread (buffer[count],1,final_chunk,pFile);
	printf("File Size read: %d, count : %d\n", result,count);

	init_packet.type = 0;
	init_packet.file_size = lSize;
	init_packet.chunk_size = data_size;
	send_buffer = (unsigned char*)malloc(sizeof(struct Init_PACKET));
	memset(send_buffer,0,sizeof(struct Init_PACKET));
	memcpy(send_buffer,(const unsigned char*)&init_packet,sizeof(init_packet));
	n=sendto(sock,send_buffer,sizeof(init_packet),0,(const struct sockaddr *)&server,length);
	if (n < 0) error("Sendto");
	free(send_buffer);
	printf("Init packet sent \n");
	
	int send_counter = 0;
	while(1)
	{
		n = recvfrom(sock,buffer1,sizeof(struct Init_PACKET),0,(struct sockaddr *)&from, &length);
		if(n > 0)
		{
			struct Init_PACKET* recv_init_packet = (struct Init_PACKET*)buffer1;
			if(recv_init_packet->type == 0)
			{
				printf("Ack of init received \n");
				break;
			}
			else
			{
				printf("sending Init packet again \n");
			}
			
		}
		else
		{
			for (send_counter = 0; send_counter<5;send_counter++)
			{
				init_packet.type = 0;
				init_packet.file_size = lSize;
				init_packet.chunk_size = data_size;
				send_buffer = (unsigned char*)malloc(sizeof(struct Init_PACKET));
				memset(send_buffer,0,sizeof(struct Init_PACKET));
				memcpy(send_buffer,(const unsigned char*)&init_packet,sizeof(init_packet));
				n=sendto(sock,send_buffer,sizeof(init_packet),0,(const struct sockaddr *)&server,length);
				if (n < 0) error("Sendto");
				free(send_buffer);
				printf("Init packet no : %d\n",count);
			}
		}
		
	}
	
  
	//struct packet* recv_packet = (struct packet*)packet_tobe_sent; 
	//printf("recv_packet chunk no  : %d\n", recv_packet->sequence_number);
	//fwrite (recv_packet->data, sizeof(char), final_chunk, oFile);
	/* the whole file is now loaded in the memory buffer. */

	// terminate
	
	
	n = recvfrom(sock,buffer1,256,0,(struct sockaddr *)&from, &length);
	if (n < 0) error("recvfrom");
	write(1,"Got an ack: \n",12);
	printf("%s\n",buffer1);

	packet_tobe_sent = (unsigned char*)malloc(sizeof(struct packet));
	int send_count = 0;
	for(send_count = 0; send_count < chunks-1; send_count++)
	{
		data_packet.sequence_number = send_count;
		memcpy(data_packet.data,buffer[send_count],data_size);
		memset(packet_tobe_sent,0,sizeof(struct packet));
		memcpy(packet_tobe_sent,(const unsigned char*)&data_packet,sizeof(data_packet));
	  
		n=sendto(sock,packet_tobe_sent,sizeof(data_packet),0,(const struct sockaddr *)&server,length);
		if (n < 0) error("Sendto");
	}
	printf("All packets sent first time \n");
	data_packet.sequence_number = -1;
	memcpy(data_packet.data,buffer[send_count],data_size);
	memset(packet_tobe_sent,0,sizeof(struct packet));
	memcpy(packet_tobe_sent,(const unsigned char*)&data_packet,sizeof(data_packet));
  
	n=sendto(sock,packet_tobe_sent,final_chunk,0,(const struct sockaddr *)&server,length);
	if (n < 0) error("Sendto");

	printf("Waiting for Ack to send last packet\n");
	n = recvfrom(sock,buffer1,256,0,(struct sockaddr *)&from, &length);
	if (n < 0) error("recvfrom");
	printf("received ack now clear to send last packet : \n");
	printf("%s\n",buffer1);


	data_packet.sequence_number = send_count;
	memcpy(data_packet.data,buffer[send_count],data_size);
	memset(packet_tobe_sent,0,sizeof(struct packet));
	memcpy(packet_tobe_sent,(const unsigned char*)&data_packet,sizeof(data_packet));
  
	n=sendto(sock,packet_tobe_sent,final_chunk,0,(const struct sockaddr *)&server,length);
	if (n < 0) error("Sendto");
	printf("size of last packet sent : %d\n",n);
	printf("Waiting for final Ack \n");
	n = recvfrom(sock,buffer1,256,0,(struct sockaddr *)&from, &length);
	if (n < 0) error("recvfrom");
	printf("last packet received at server : \n");
	printf("%s\n",buffer1);
	
	close(sock);
	fclose (pFile);
	free (buffer);
	return 0;
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}
