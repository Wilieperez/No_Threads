#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "SC.h"

pthread_t thread;
pthread_mutex_t mutex;
char source[15];
char destination[2][15];
int clients[BACKLOG];

void *handle_client() {
   int read_size;

   frame_msg server_frame;
   frame_msg client_frame;
   server_frame.preamb = PREAMB;

    while(1){
      if((read_size = recv(clients[0] ,(char *)&client_frame , sizeof(frame_msg) , 0)) > 0){
         printf("Client 0: %i-%s-%s-%s\n", client_frame.preamb, client_frame.source, client_frame.destination, client_frame.payload);

         //Forming response
         strcpy(server_frame.source, source);
         strcpy(server_frame.destination, destination[0]);
         strcpy(server_frame.payload, "Message recieved");

         if(send(clients[0], &server_frame, sizeof(frame_msg), 0) < 0) {
            perror("Send failed\n");
         }

         if(read_size == 0) {
            puts("Client disconnected\n");
            clients[0] = 0;
         } else if(read_size == -1) {
            perror("Receive failed\n");
         }
      }
      if((read_size = recv(clients[1] ,(char *)&client_frame , sizeof(frame_msg) , 0)) > 0){
         printf("Client 1: %i-%s-%s-%s\n",client_frame.preamb, client_frame.source, client_frame.destination, client_frame.payload);

         //Forming response
         strcpy(server_frame.source, source);
         strcpy(server_frame.destination, destination[1]);
         strcpy(server_frame.payload, "Message recieved");

         if(send(clients[1], &server_frame, sizeof(frame_msg), 0) < 0) {
            perror("Send failed\n");
         }

         if(read_size == 0) {
            puts("Client disconnected\n");
            clients[1] = 0;
         } else if(read_size == -1) {
            perror("Receive failed\n");
         }
      }
    }
      pthread_mutex_unlock(&mutex);

   if(close(clients[0]) < 0 || close(clients[1]) < 0) {
      perror("Close socket failed\n");
   }

   pthread_exit(NULL);
}

int main(int argc, char **argv) {
   int sockfd, newfd;
   struct sockaddr_in host_addr, client_addr;
   socklen_t sin_size;

   if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("Socket failed");
      exit(1);
   }
   puts("--Socket created\n");

   host_addr.sin_family = AF_INET;
   host_addr.sin_port = htons(PORT);
   host_addr.sin_addr.s_addr = INADDR_ANY;
   memset(&(host_addr.sin_zero), '\0', 8);

   if(bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1) {
      perror("Bind failed");
      exit(1);
   }
   puts("--Address binded\n");

   if(listen(sockfd, BACKLOG) == -1) {
      perror("Listen failed");
      exit(1);
   }
   puts("--Listening...\n");

   sin_size = sizeof(struct sockaddr_in);
   
   if(pthread_create(&thread, NULL, handle_client, NULL) < 0) {
      perror("Thread creation failed");
      exit(1);
   }

   while(1) {
         if((newfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size)) > 0){   
            for(int i = 0; i < BACKLOG; i++){
               if(clients[i] == 0){
                  clients[i] = newfd;
                  printf("Client found! Address: %s\n\n", inet_ntoa(client_addr.sin_addr));
                  strcpy(destination[i], inet_ntoa(client_addr.sin_addr));
                  
                  if(send(newfd, destination[i], sizeof(destination[0]), 0) < 0) {
                     perror("Address send failed\n");
                  }
                  break;
               }
            }      
         }
   }

   

   if(close(sockfd) < 0) {
      perror("Close socket failed\n");
      exit(1);
   }

   return 0;
}
