#ifndef CLIENTE_H_INCLUDED
#define CLIENTE_H_INCLUDED

typedef struct client_t
{
    int socket_number;
    int thread_number;
    char buffer_client[512];
} Client;

#endif