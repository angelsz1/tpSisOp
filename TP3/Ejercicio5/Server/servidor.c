// #APL N. 3
// #EJERCICIO 5
// #INTEGRANTES:
//       #Carballo, Facundo Nicolas (DNI: 42774931)
//       #Garcia Burgio, Matias Nicolas (DNI: 42649117)
//       #Mottura, Agostina Micaela (DNI: 41898101)
//       #Povoli Olivera, Victor (DNI: 43103780)
//       #Szust, Ángel Elías (DNI: 43098495)

#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <semaphore.h>

#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#include "socket.h"
#include "server.h"

#define SIZE_QUERY 512

void ayuda()
{
    puts(COLOR_GREEN "-------------------------------------------------- AYUDA DEL SCRIPT --------------------------------------------------" COLOR_RESET);
    puts("Con este script crearas un servidor que estara escuchando peticiones de un cliente");
    puts("Para ejecutarlo de manera correcta deberas ejecutar el siguiente comando:");
    puts(COLOR_GREEN "./servidor [PUERTO]" COLOR_RESET);
    puts("EJEMPLO:");
    puts(COLOR_GREEN"./servidor 5555"COLOR_RESET);
    puts("El cliente tiene 3 tipos de peticiones para hacerle a este servidor:");
    puts(COLOR_YELLOW "- ALTA:" COLOR_RESET);
    puts("       - Persiste la informacion de un gato en gatos.txt (Si el gato ya existe te informara por pantalla)");
    puts(COLOR_YELLOW "- BAJA:" COLOR_RESET);
    puts("       - Elimina la informacion de un gato en gatos.txt (Si el gato no existiese te informara por pantalla)");
    puts(COLOR_YELLOW "- CONSULTA:" COLOR_RESET);
    puts("       - Devuelve la informacion de los gatos que se encuentran en gatos.txt");
    puts("       - Devuelve la informacion de un gato en especifico (Si el gato no existiese te informara por pantalla)");
    puts(COLOR_YELLOW "IMPORTANTE:" COLOR_RESET);
    puts("       - De querer finalizar el proceso tendran que hacerlo atraves de la senial SIGUSR1 (haciendole un kill -SIGUSR1 [PID])");
    puts(COLOR_GREEN "----------------------------------------------------------------------------------------------------------------------" COLOR_RESET);
}

void *handle_thread(void *);
void *handle_thread_error(void *);

int thread_number = 3;
int active = 1;
int socket_server = -1;

void handle_signal()
{
    active = 0;
    
    if (socket_server > 0)
    {
        close(socket_server);
    }
}

void daemon_config()
{
    int pid;

    // Ignoramos la senial SIGINT (CTRL + C)
    signal(SIGINT, SIG_IGN);

    // Hacemos que la senial SIGUSR1 termine el servidor
    signal(SIGUSR1, &handle_signal);

    pid = fork();



    if (pid > 0)
    {
        printf("PID: %d\n", pid);
        exit(0); // Finalizamos el proceso padre. Logrando que el proceso que creamos se ejecute en segundo plano
    }
    else if (pid < 0)
    {
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int socket_client, pid;
    unsigned int len;
    struct sockaddr_in server_address, client_address;

    if (argc > 1 && (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        ayuda();
        return 0;
    }

    if (argc != 2)
    {
        puts(COLOR_RED "Argumentos mal ingresados" COLOR_RESET);
        ayuda();
        return 1;
    }

    daemon_config();

    // Creamos el socket
    socket_server = socket_create();

    server_address = socket_config(argv[1]);

    socket_bind(socket_server, server_address);

    socket_listen(socket_server, &server_address);

    len = sizeof(client_address);

    sem_t *sem_file = sem_open("socket-rw-file", O_CREAT, 0666, 1);

    while (active)
    {
        socket_client = accept(socket_server, (struct sockaddr *)&client_address, &len);

        pthread_t thread;
        if (thread_number == 0)
        {
            pthread_create(&thread, NULL, handle_thread_error, &socket_client);
        }
        else
        {
            pthread_create(&thread, NULL, handle_thread, &socket_client);
            thread_number--;
        }
    }

    sem_close(sem_file);
    sem_unlink("socket-rw-file");
    close(socket_server);
}

void *handle_thread(void *p_socket_client)
{
    int socket_client = *((int *)p_socket_client);
    Client client;
    char buffer_client[SIZE_QUERY];

    strcpy(client.buffer_client, "CONEXION EXITOSA.");
    write(socket_client, client.buffer_client, strlen(client.buffer_client));

    sem_t *sem_file = sem_open("socket-rw-file", O_CREAT, 0666, 1);

    if (socket_client < 0)
    {
        fprintf(stderr, COLOR_RED "Conexion no aceptada. %d: %s\n", errno, strerror(errno));
        return NULL;
    }

    while (1)
    {
        ssize_t len_rx = read(socket_client, client.buffer_client, sizeof(client.buffer_client));

        if (len_rx == -1)
        {
            fprintf(stderr, COLOR_RED "Error al leer. %d: %s \n", errno, strerror(errno));
        }
        else if (len_rx == 0)
        {
            close(socket_client);
            thread_number++;
            break;
        }
        else
        {

            client.socket_number = socket_client;
            client.thread_number = thread_number;

            sem_wait(sem_file);
            server_read_query(&client);
            sem_post(sem_file);

            thread_number++;
        }
    }
}

void *handle_thread_error(void *p_socket_client)
{
    int socket_client = *((int *)p_socket_client);
    // Client client = *((Client *)p_socket_client);
    Client client;

    strcpy(client.buffer_client, "SERVIDOR COLAPSADO.");
    write(socket_client, client.buffer_client, strlen(client.buffer_client));
}