#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "SC.h"

pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
   int fd, numbytes;
   char buf[15];
   struct sockaddr_in server;
   struct hostent *lh;//localhost

   //Frames
   frame_msg client_frame;
   frame_msg server_frame;
   client_frame.preamb = PREAMB;

   if ((fd = socket(AF_INET, SOCK_STREAM, 0))==-1){
      printf("failed to create socket\n");
      exit(-1);
   }
   puts("--Socket created\n");

   if ((lh=gethostbyname("localhost")) == NULL){
      printf("failed to get hostname\n");
      exit(-1);
   }//obtain localhost

   server.sin_family = AF_INET;
   server.sin_port = htons(PORT);
   server.sin_addr = *((struct in_addr *)lh->h_addr);
   bzero(&(server.sin_zero),8);

   if(connect(fd, (struct sockaddr *)&server,  sizeof(struct sockaddr))==-1){
      printf("failed to connect\n");
      exit(-1);
   }
   if((numbytes=recv(fd, buf, sizeof(buf), 0)) == -1){
      printf("failed to recieve IP");
      return 1;
   }
   printf("--Connected using IP: %s\n", buf);

   while(1){
         printf("Message for Server: ");
         scanf("%s",&client_frame.payload);
         strcpy(client_frame.source, buf);
         strcpy(client_frame.destination, inet_ntoa(server.sin_addr));

         pthread_mutex_lock(&mutex);

         send(fd,(char *)&client_frame,sizeof(frame_msg),0);//Request frame

      if ((numbytes=recv(fd,(char *)&server_frame,sizeof(frame_msg),0)) == -1){
         printf("failed to recieve\n");
         continue;
      }//Response frame

      system("clear");
      printf("\nFrame:\n\n");

      printf("%X-%s-%s-%s\n",server_frame.preamb,server_frame.source,server_frame.destination, server_frame.payload);

      pthread_mutex_unlock(&mutex);
   }
   return 0;
}
