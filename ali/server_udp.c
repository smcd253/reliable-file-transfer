

#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <math.h>

#define data_size 60000


void error(const char *msg)
{
    perror(msg);
    exit(0);
}

struct Init_PACKET{
  unsigned char type;
  u_int file_size;
  u_int chunk_size;
};

struct packet{
  unsigned char type;
  int sequence_number;
  unsigned char data[data_size];
};

struct ack_packet{
  unsigned char type;
  bool * packet_received;
};



int main(int argc, char *argv[])
{
  FILE * oFile;
   int sock, length, n;
   socklen_t fromlen;
   struct sockaddr_in server;
   struct sockaddr_in from;
   char buf[63000];
   int chunks, final_chunk;
   bool init_check = 0, mem_alloc_ch=0;
   int cons_check = 0;  
   
   if (argc < 2) {
      fprintf(stderr, "ERROR, no port provided\n");
      exit(0);
   }
   
   sock=socket(AF_INET, SOCK_DGRAM, 0);
   if (sock < 0) error("Opening socket");
   length = sizeof(server);
   bzero(&server,length);
   server.sin_family=AF_INET;
   server.sin_addr.s_addr=INADDR_ANY;
   server.sin_port=htons(atoi(argv[1]));
   if (bind(sock,(struct sockaddr *)&server,length)<0) 
       error("binding");
   fromlen = sizeof(struct sockaddr_in);

   int file_sz;
   int send_counter = 0;
   printf("Waiting for init packet \n");
   while (1) 
   { 


      if(init_check == 0)
      {
         n = recvfrom(sock,buf,sizeof(struct Init_PACKET),0,(struct sockaddr *)&from,&fromlen);
         if (n < 0) error("recvfrom");
         if(n > 0)
         {

            printf("No of bytes received: %d\n",n);
           struct Init_PACKET* recv_init_packet = (struct Init_PACKET*)buf;
           if(recv_init_packet->type == 0)
           {
            init_check = 1;
            printf("Init packet received decoding ... \n");
              file_sz = recv_init_packet->file_size;
             printf("File size : %d\n",file_sz);
             printf("Data size : %d\n",recv_init_packet->chunk_size);
           }
         }
       }

       else if(init_check == 1 && mem_alloc_ch == 0)
       {
        chunks = ceil(file_sz/(float)data_size);
        char **buffer = (char**) malloc (chunks*sizeof(char*));
        int count = 0;
        for(count = 0; count < chunks - 1;count++)
        {
          buffer[count] = (char*) malloc (data_size*sizeof(char));
        } 
        final_chunk = file_sz - (chunks-1)*data_size;
        buffer[count] = (char*) malloc (final_chunk*sizeof(char));
        mem_alloc_ch = 1;
       }
       else
       {
          n = recvfrom(sock,buf,data_size,0,(struct sockaddr *)&from,&fromlen);
          if(n > 0)
          {
            struct packet* data_packet = (struct packet*)buf;
            if(data_packet->type ==0)
            {
              n = sendto(sock,buf,sizeof(struct Init_PACKET),0,(struct sockaddr *)&from,fromlen);
              if (n  < 0) error("sendto");
            }
            else if(data_packet->type == 1)
            {
              struct packet* data_packet = (struct packet*)buf;
               printf("Received seq no : %d\n",data_packet->sequence_number);
               memcpy(buffer[data_packet->sequence_number],data_packet->data,data_size);
            }
            else if(data_packet->type == 2)
            {
              if(packet_counter == chunks-2)
              {
                
              }

            }
          } 
       }
       

       
       
       break;
   }
  
  
  printf("Waiting for data..\n");
  

/*
  int packet_counter = 1; 
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
  return 0;
 }







