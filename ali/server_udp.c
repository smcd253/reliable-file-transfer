#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define data_size 60000


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

struct Init_PACKET{
  uint8_t type;
  u_int file_size;
  u_int chunk_size;
};

struct packet{
  uint8_t type;
  int sequence_number;
  unsigned char data[data_size];
};

struct ack_packet{
  uint8_t type;
  char packet_tracker[20000];
};


int main(int argc, char *argv[])
{
  FILE * oFile;
  FILE * f1;
  int sock, length, n;
  socklen_t fromlen;
  struct sockaddr_in server;
  struct sockaddr_in from;
  char buf[63000];
  char send_buffer[10000];
  int chunks, final_chunk;
  bool init_check = 0, mem_alloc_ch=0, data_check=0;
  int cons_check = 0;
  int type; 
  struct ack_packet ack_packet1;
  int packet_counter = 0; 

  f1 = fopen ( "log_server.txt" , "w" );
  
  /*if (argc < 2) 
  {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(0);
  }*/
  sock=socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) error("Opening socket");
  length = sizeof(server);
  bzero(&server,length);
  server.sin_family=AF_INET;
  server.sin_addr.s_addr=INADDR_ANY;
  server.sin_port=htons(atoi("12345"));
  if (bind(sock,(struct sockaddr *)&server,length)<0) 
    error("binding");

  fromlen = sizeof(struct sockaddr_in);

  int file_sz;
  int send_counter = 0;
  printf("Waiting for init packet \n");
  while(1)
  {
    n = recvfrom(sock,buf,sizeof(struct Init_PACKET),0,(struct sockaddr *)&from,&fromlen);
    if (n < 0) error("recvfrom");
    
    printf("No of bytes received: %d\n",n);
    struct Init_PACKET* recv_init_packet = (struct Init_PACKET*)buf;
    if(recv_init_packet->type == 0)
    {
      init_check = 1;
      printf("Init packet received decoding ... \n");
      file_sz = recv_init_packet->file_size;
      printf("File size : %d\n",file_sz);
      printf("Data size : %d\n",recv_init_packet->chunk_size);
      break;
    }
  }
  
  printf("allocating memory\n");
  chunks = ceil(file_sz/(float)data_size);
  char **buffer = (char**) malloc (chunks*sizeof(char*));
  int count = 0;
  for(count = 0; count < chunks - 1;count++)
  {
    buffer[count] = (char*) malloc (data_size*sizeof(char));
  } 
  final_chunk = file_sz - (chunks-1)*data_size;
  buffer[count] = (char*) malloc (final_chunk*sizeof(char));
  
  
  ack_packet1.type = 4;
  int b=0;
  printf("Type packet1 %d\n",ack_packet1.type);
    for(b = 0; b<chunks;b++)
    {
        printf("%d",ack_packet1.packet_tracker[b]);
    }
    printf("\n");

  memcpy(send_buffer,(unsigned char*)&ack_packet1,sizeof(ack_packet1));
  n = sendto(sock,send_buffer,sizeof(ack_packet1),0,(struct sockaddr *)&from,fromlen);
  if (n  < 0) error("sendto");
  mem_alloc_ch = 1;
  printf("allocating memory done \n");
  printf("Now waiting for data\n");
  while (1) 
  {
    n = recvfrom(sock,buf,data_size,0,(struct sockaddr *)&from,&fromlen);
    struct packet* data_packet = (struct packet*)buf;
    type = data_packet->type;
    if(type == 0)
    {
      ack_packet1.type = 0;
      memcpy(send_buffer,(unsigned char*)&ack_packet1,sizeof(ack_packet1));
      n = sendto(sock,send_buffer,sizeof(ack_packet1),0,(struct sockaddr *)&from,fromlen);
      if (n  < 0) error("sendto");
    }
    else if(type == 1)
    {

      struct packet* data_packet = (struct packet*)buf;
      if(ack_packet1.packet_tracker[data_packet->sequence_number] <= 0)
      {
        printf("Received seq no : %d\n",data_packet->sequence_number);
        memcpy(buffer[data_packet->sequence_number],data_packet->data,data_size);
        ack_packet1.packet_tracker[data_packet->sequence_number] = 1;
        packet_counter++;
      }
    }
    else if(type == 2)
    {
      printf("Packet counter val %d\n",packet_counter);
      //fprintf(f1,"Packet counter val %d\n",packet_counter);
      
      int b;
      
      fprintf(f1,"\n");
      if(packet_counter >= (chunks-2))
      {
        printf("All packets received except last \n");
        ack_packet1.type = 3;
        memcpy(send_buffer,(unsigned char*)&ack_packet1,sizeof(ack_packet1));
        n = sendto(sock,send_buffer,sizeof(ack_packet1),0,(struct sockaddr *)&from,fromlen);
        if (n  < 0) error("sendto");
        fprintf(f1, "p: ");
        for(b = 0; b<chunks;b++)
        {
          fprintf(f1,"%d",ack_packet1.packet_tracker[b]);

        }
        fprintf(f1,"\n");
        break;
      }
      else
      {
        
        ack_packet1.type = 2;
        int b;
        
        memcpy(send_buffer,(unsigned char*)&ack_packet1,chunks*sizeof(char));

        for(b = 0; b<chunks;b++)
        {
          fprintf(f1,"%d",ack_packet1.packet_tracker[b]);

        }
        fprintf(f1,"\n");
        n = sendto(sock,send_buffer,sizeof(struct ack_packet),0,(struct sockaddr *)&from,fromlen);
        if (n  < 0) error("sendto");
        //fprintf(f1, "before sending\n");
        
        fprintf(f1,"\n");
        fprintf(f1,"\n");

      }

    }
  }
  while(1)
  {
    printf("Waiting for last packet\n");
    n = recvfrom(sock,buf,final_chunk,0,(struct sockaddr *)&from,&fromlen);
    printf("No of bytes received %d\n",n );
    struct packet* data_packet = (struct packet*)buf;
    printf("type %d\n",data_packet->type );
    printf("Received seq no : %d\n",data_packet->sequence_number);
    printf("1\n");
    memcpy(buffer[data_packet->sequence_number],data_packet->data,final_chunk-1);
    printf("2\n");
    int s_count =0;
    for(s_count = 0;s_count<2;s_count++)
    {
      ack_packet1.type = 5;
      memcpy(send_buffer,(unsigned char*)&ack_packet1,sizeof(ack_packet1));
      n = sendto(sock,send_buffer,sizeof(ack_packet1),0,(struct sockaddr *)&from,fromlen);
      if (n  < 0) error("sendto");
    }
    break;
  }
  
  oFile = fopen ( "file/received_test1.mov" , "wb" );
printf("3\n");
  int w_count = 0;
  for(w_count = 0; w_count<chunks-1;w_count++)
  {
    fwrite (buffer[w_count] , sizeof(char), data_size, oFile);
  }
  printf("4\n");
  fwrite (buffer[w_count], sizeof(char), final_chunk, oFile);
  printf("5\n");

/*
  
  while(1)
  {
    
     n = recvfrom(sock,buf,data_size,0,(struct sockaddr *)&from,&fromlen);
     if (n < 0) error("recvfrom");

     struct packet* data_packet = (struct packet*)buf;
     printf("Received seq no : %d\n",data_packet->sequence_number);
     if(data_packet->sequence_number == -1)
     {
      break;
     }
     memcpy(buffer[data_packet->sequence_number],data_packet->data,data_size);
     if(packet_counter >= chunks-1)
     {
      break;
     }
     
     packet_counter++;
  }
  printf("Packets received: %d\n",packet_counter);
  while(1)
  {
    //printf("stopped\n");
  }
  n = sendto(sock,"send last packet\n",20,0,(struct sockaddr *)&from,fromlen);
       if (n  < 0) error("sendto");

  n = recvfrom(sock,buf,final_chunk,0,(struct sockaddr *)&from,&fromlen);
  struct packet* data_packet = (struct packet*)buf;
   printf("Received seq no : %d\n",data_packet->sequence_number);
   memcpy(buffer[data_packet->sequence_number],data_packet->data,data_size);

   n = sendto(sock,"last packet received\n",20,0,(struct sockaddr *)&from,fromlen);
       if (n  < 0) error("sendto");

  oFile = fopen ( "file/received_test1.mov" , "wb" );

   /*int w_count = 0;
  for(w_count = 0; w_count<chunks-1;w_count++)
  {
    fwrite (buffer[w_count] , sizeof(char), data_size, oFile);
  }
  
  fwrite (buffer[w_count], sizeof(char), final_chunk, oFile);*/
  fclose (oFile);
  close(sock);
  
  return 0;
}