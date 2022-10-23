#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include "lista.h"
#include <memory.h>
#include <signal.h>

typedef struct {
    char accion[9]; 
    char nombre[20];
    char raza[20];
    char sexo;
    char condicion[2];
} Pedido;

typedef struct {
    char nombre[20];
    char raza[20];
    char sexo;
    char condicion[2];
} Gato;

typedef struct {
    int status; 
    char contenido[2000];
} Respuesta;

void crearServidor();
void cerrarServidor(Pedido* pedido, Respuesta* respuesta, int shmid, sem_t* semComando, sem_t* semRespuesta, Lista* listaGatos);
void signalHandler(int signal);
int crearMemoriaCompartida();
int compararGato(const void* gato1, const void* gato2);
void alta(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta);
void baja(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta);
void consulta(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta);
void error(Pedido* pedido, Respuesta* respuesta);

int serverActivo = 1;

int main()
{
    crearServidor();
    if (signal(SIGUSR1, signalHandler) == SIG_ERR)
        printf("No se pudo capturar la senal SIGUSR1\n");
    if (signal(SIGINT, signalHandler) == SIG_ERR)
        printf("No se pudo capturar la senal SIGUSR1\n");

    Lista listaGatos;
    crearLista(&listaGatos);

    sem_t* semComando = sem_open("/comando", O_CREAT, 0666, 0);
    sem_t* semRespuesta = sem_open("/respuesta", O_CREAT, 0666, 0);
    if (semComando == SEM_FAILED || semRespuesta == SEM_FAILED) {
        sem_close(semComando);
        sem_close(semRespuesta);
        printf("Error creando el semaforo");
        exit(-1);
    }

    int shmid = crearMemoriaCompartida();
    Pedido* pedido = (Pedido*)shmat(shmid,NULL,0);
    Respuesta* respuesta = (Respuesta*)shmat(shmid,NULL,0);
   
    while (serverActivo)
    {
        sem_wait(semComando);

        if(strcmp("ALTA", pedido->accion) == 0)
            alta(&listaGatos, pedido, respuesta);

        else if(strcmp("BAJA", pedido->accion) == 0)
            baja(&listaGatos, pedido, respuesta);

        else if(strcmp("CONSULTA", pedido->accion) == 0)
            consulta(&listaGatos, pedido, respuesta);
        
        else
            error(pedido, respuesta);

        sem_post(semRespuesta);
    }

    cerrarServidor(pedido, respuesta, shmid, semComando, semRespuesta,  &listaGatos);

    return 0;
}

void cerrarServidor(Pedido* pedido, Respuesta* respuesta, int shmid, sem_t* semComando, sem_t* semRespuesta, Lista* listaGatos) {
    shmdt(&pedido);
    shmdt(&respuesta);
    shmctl(shmid, IPC_RMID, NULL);

    sem_close(semComando);
    sem_close(semRespuesta);

    sem_unlink("/comando");
    sem_unlink("/respuesta");
    vaciarLista(listaGatos);
}

void crearServidor() {
    pid_t process_id = 0;

    process_id = fork();

    if(process_id < 0) {
        printf("fork failed!\n");
        exit(1);
    }

    if (process_id > 0) {
        printf("process_id of child process %d \n", process_id);
        exit(0);
    }
}

void signalHandler(int signal)
{
    if (signal == SIGUSR1) {
        serverActivo = 0;
    }
}

int crearMemoriaCompartida() {
    char* clave = "../refugio";
    key_t key = ftok(clave, 4);
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);

    if (shmid == -1) {
        printf("Hubo un error al intentar abrir el área de memoria compartida.");
        exit(1);
    }

    return shmid;
}

int compararGato(const void* gato1, const void* gato2) {
    Gato g1 = *(Gato*)gato1;
    Gato g2 = *(Gato*)gato2;
    return strcmp(g1.nombre, g2.nombre);
}

void alta(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta) {
    printf("Haciendo el alta");
    Gato gato;
    strcpy(gato.nombre, pedido->nombre);
    strcpy(gato.raza, pedido->raza);
    gato.sexo = pedido->sexo;
    strcpy(gato.condicion, pedido->condicion);

    int resultado = insertarEnListaOrdenada(listaGatos, &gato, sizeof(Gato), compararGato);

    if(resultado == 1) {
        respuesta->status = 200;
        strcpy(respuesta->contenido, gato.nombre);
        strcat(respuesta->contenido, " ingresado correctamente");
        printf("status: %d", respuesta->status);
    } else if (resultado == 2){
        respuesta->status = 409;
        strcpy(respuesta->contenido, gato.nombre);
        strcat(respuesta->contenido, " ya existe");
    }     
}

void baja(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta) {
    Gato gato;
    strcpy(gato.nombre, pedido->nombre);
    eliminarDeLista(listaGatos, &gato, sizeof(Gato), compararGato);

    respuesta->status = 204;
    strcpy(respuesta->contenido, gato.nombre);
    strcat(respuesta->contenido, " eliminado correctamente");             
}

void consulta(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta) {
    Gato gato;
    strcpy(gato.nombre, pedido->nombre);

    if(strcmp("", pedido->nombre) == 0) {
        respuesta->status = 200;
        listarTodos(listaGatos, respuesta->contenido);
    } else {
        int encontrado = buscarEnLista(listaGatos, &gato, sizeof(Gato), compararGato);
        if(encontrado) {
            respuesta->status = 200;
            char mensaje[200];
            char aux[70];

            sprintf (mensaje, "|%12s|%12s|%5s|%10s|\n", 
            "Nombre", "Raza", "Sexo", "Condicion");
            sprintf (aux, "|%12s|%12s|%5c|%10s|\n", 
            gato.nombre, gato.raza, gato.sexo, gato.condicion);

            strcat(mensaje, aux);
            strcpy(respuesta->contenido, mensaje);
        } else {
            respuesta->status = 404;
            strcpy(respuesta->contenido, gato.nombre);
            strcat(respuesta->contenido, " no encontrado");
        }
    }
}

void error(Pedido* pedido, Respuesta* respuesta) {
    char accion[9];
    strcpy(accion, pedido->accion);
    respuesta->status = 400;
    strcpy(respuesta->contenido, "BAD REQUEST: accion no valida ");
    strcat(respuesta->contenido, accion);
}