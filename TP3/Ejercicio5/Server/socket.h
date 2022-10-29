#ifndef SOCKET_H
#define SOCKET_H

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/stat.h>
#include <stdlib.h>

#define MAX_QUEUE 1

int socket_create();
void socket_bind(int server_socket, struct sockaddr_in server_address);
void socket_listen(int server_socket, struct sockaddr_in* p_server_address);

struct sockaddr_in socket_config(char *);

#endif