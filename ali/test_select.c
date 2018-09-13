#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>
#include <sys/time.h>

#define MAXSIZE 100

main(int argc, char *argv[])
{
   int s;			   /* s = socket */
   struct sockaddr_in in_addr;	   /* Structure used for bind() */
   struct sockaddr_in sock_addr;   /* Output structure from getsockname */
   struct sockaddr_in src_addr;    /* Used to receive (addr,port) of sender */
   int src_addr_len;		   /* Length of src_addr */
   int len;			   /* Length of result from getsockname */
   char line[MAXSIZE];

   if (argc == 1)
    { printf("Usage: %s port\n", argv[0]);
      exit(1);
    }

   /* ---
      Create a socket
      --- */
   s = socket(PF_INET, SOCK_DGRAM, 0);

   /* ---
      Set up socket end-point info for binding
      --- */
   in_addr.sin_family = AF_INET;                   /* Protocol family */
   in_addr.sin_addr.s_addr = htonl(INADDR_ANY);    /* Wildcard IP address */
   in_addr.sin_port = htons(atoi(argv[1]));	   /* Use any UDP port */

   /* ---
      Here goes the binding...
      --- */
   if (bind(s, (struct sockaddr *)&in_addr, sizeof(in_addr)) == -1)
    { perror("bind: ");
      printf("- Note: this program can be run on any machine...\n");
      printf("        but you have to pick an unsed port#\n");
      exit(1);
    }
   else
    { printf("OK: bind SUCCESS\n");
    }

   /* ------
      Receive loop
      ------ */
    struct timeval timeout={2,0};
    setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
   while(1)
    { 
      src_addr_len = sizeof(src_addr);
      len = recvfrom(s, line, MAXSIZE, 0  ,(struct sockaddr *) &src_addr, &src_addr_len);
      if(len >0 )
      {
        printf("msg recvd\n");
      }
      else
      {
        printf("timeout\n");
      }

        
      /*struct pollfd fds[1];		// fd's used by poll 
      int    timeout;                   // Time value for time out 
      int    n;

      fds[0].fd = s;
      fds[0].events = 0;
      fds[0].events |= POLLIN;		// Set the interesting events 

      timeout = 1000;			// Timeout set for 5000 milli sec

      n = poll(fds, 1, timeout);  	// Wait timeout 

      if ( n == 0 )
       { printf("Poll has timed out...\n");
       }
      else
       { src_addr_len = sizeof(src_addr);
         len = recvfrom(s, line, MAXSIZE, 0 // flags ,
                    (struct sockaddr *) &src_addr, &src_addr_len);

         printf("Msg from (%u,%u): `%s' (%u bytes)\n", 
		src_addr.sin_addr.s_addr, src_addr.sin_port, line, len);
       }*/
    }
}