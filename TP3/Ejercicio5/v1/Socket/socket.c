#include "socket.h"

int socket_create()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1)
    {
        puts("No se pudo crear el socket.");
        exit(1);
    }

    return server_socket;
}

void socket_bind(int server_socket, struct sockaddr_in *p_server_address)
{
    if (bind(server_socket, (struct sockaddr *)&p_server_address, sizeof(struct sockaddr_in)) < 0)
    {
        puts("No se pudo vincular el socket con la configuración del servidor.");
        exit(1);
    }
}

void socket_listen(int server_socket, struct sockaddr_in* p_server_address)
{
    if ((listen(server_socket, MAX_QUEUE)) != 0)
    {
        fprintf(stderr, "Error en la esucha del servidor. %d: %s \n", errno, strerror(errno));
        exit(1);
    }
    else
    {
        printf("Servidor escuchando en el puerto %d \n\n", ntohs(p_server_address->sin_port));
    }
}

struct sockaddr_in socket_config(char *port)
{
    struct sockaddr_in serverAddress;

    // Limpiamos la estructura
    memset(&serverAddress, 0, sizeof(struct sockaddr_in));

    // Dominio
    serverAddress.sin_family = AF_INET;

    // Puerto
    serverAddress.sin_port = htons(atoi(port));

    // Adress a la que escuchará el server (en este caso, cualquiera)
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    return serverAddress;
}