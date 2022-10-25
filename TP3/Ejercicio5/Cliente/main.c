#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>

#include "socket.h"

#define SERVER_ADDRESS  "192.168.0.20"     /* server IP */
#define PORT            3693 

/* Test sequences */
char buf_tx[512];     
char buf_rx[512];                     /* receive buffer */
 
 
/* This clients connects, sends a text and disconnects */
int main(int argc, char *argv[]) 
{ 
    int server_socket;//sockfd; 
    struct sockaddr_in server_address; 
    
    server_socket = socket_create();
    
    server_address = socket_config("3693");

    socket_connect(server_socket, server_address);    
      
    // Copiamos en el buffer el mensaje a enviar
    strcpy(buf_tx, argv[1]);
    
    // Escribimos el mensaje
    write(server_socket, buf_tx, sizeof(buf_tx));     
    
    // Leemos la respuesta del servidor
    read(server_socket, buf_rx, sizeof(buf_rx));


    printf("Respuesta del servidor: %s \n", buf_rx);
   
    close(server_socket); 
} 