/*standard symbols */
#include <unistd.h>

/* sockets */
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

/* strings / errors*/
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "socket.h"

#define MAX_QUEUE 16
#define SIZE_QUERY 512

/* server parameters */
#define SERV_PORT 3693                /* port */
#define SERV_HOST_ADDR "192.168.0.20" /* IP, only IPV4 support  */
#define BUF_SIZE 100                  /* Buffer rx, tx max size  */
#define BACKLOG 5                     /* Max. client pending connections  */

int main(int argc, char *argv[])
{
    int server_socket, connfd;
    unsigned int len;
    struct sockaddr_in server_address, client_address;
    int len_rx, len_tx = 0;
    char buffer_server[SIZE_QUERY];
    char buffer_client[SIZE_QUERY];

    // Creamos el socket
    server_socket = socket_create();

    server_address = socket_config("3693");

    socket_bind(server_socket, server_address);


    socket_listen(server_socket, &server_address);


    len = sizeof(client_address);

    while (1)
    {
        connfd = accept(server_socket, (struct sockaddr *)&client_address, &len);
        if (connfd < 0)
        {
            fprintf(stderr, "[SERVER-error]: connection not accepted. %d: %s \n", errno, strerror(errno));
            return -1;
        }

        while (1)
        {
            /* read client message, copy it into buffer */
            len_rx = read(connfd, buffer_client, sizeof(buffer_client));

            if (len_rx == -1)
            {
                fprintf(stderr, "[SERVER-error]: connfd cannot be read. %d: %s \n", errno, strerror(errno));
            }
            else if (len_rx == 0) /* if length is 0 client socket closed, then exit */
            {
                printf("[SERVER]: client socket closed \n\n");
                close(connfd);
                break;
            }
            else
            {
                printf("Receive: %s\n", buffer_client);
                strcpy(buffer_server, "GIL");
                write(connfd, buffer_server, strlen(buffer_server));
                printf("[SERVER]: %s \n", buffer_server);
            }
        }
    }
}