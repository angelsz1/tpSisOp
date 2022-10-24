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
#include "../global.h"

#define ELIMINADO 1
#define NOT_FOUND 0

void crearServidor();
void cerrarServidor(Pedido* pedido, Respuesta* respuesta, int shmid, sem_t* semComando, sem_t* semRespuesta, Lista* listaGatos);
void signalHandler(int signal);
int crearMemoriaCompartida();

void alta(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta);
void baja(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta);
void consulta(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta);
void error(Pedido* pedido, Respuesta* respuesta);
void atraparSeniales();

int serverActivo = 1;

int main()
{
    crearServidor();
    atraparSeniales();

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

void alta(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta) {
    Gato gato;
    strcpy(gato.nombre, pedido->nombre);
    strcpy(gato.raza, pedido->raza);
    gato.sexo = pedido->sexo;
    strcpy(gato.condicion, pedido->condicion);

    int resultado = insertarEnListaOrdenada(listaGatos, &gato, sizeof(Gato));

    if(resultado == 1) {
        respuesta->status = 200;
        strcpy(respuesta->contenido, gato.nombre);
        strcat(respuesta->contenido, " ingresado correctamente");
    } else if (resultado == 2){
        respuesta->status = 409;
        strcpy(respuesta->contenido, gato.nombre);
        strcat(respuesta->contenido, " ya existe");
    }
}

void baja(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta) {
    Gato gato;
    strcpy(gato.nombre, pedido->nombre);
    int resultado = eliminarDeLista(listaGatos, &gato, sizeof(Gato));
    if(resultado == ELIMINADO) {
        respuesta->status = 204;
        strcpy(respuesta->contenido, gato.nombre);
        strcat(respuesta->contenido, " eliminado correctamente");
    } 
    else if (resultado == NOT_FOUND) {
        respuesta->status = 404;
        strcpy(respuesta->contenido, gato.nombre);
        strcat(respuesta->contenido, " no existe");
    }
}

void consulta(Lista* listaGatos, Pedido* pedido, Respuesta* respuesta) {
    Gato gato;
    strcpy(gato.nombre, pedido->nombre);

    if(strcmp("", pedido->nombre) == 0) {
        respuesta->status = 200;
        listarTodos(listaGatos, respuesta->contenido);
    } else {
        int encontrado = buscarEnLista(listaGatos, &gato, sizeof(Gato));
        if(encontrado) {
            respuesta->status = 200;
            char mensaje[200];
            char aux[70];

            sprintf (mensaje, ANSI_COLOR_GREEN"|%12s|%12s|%5s|%10s|\n", 
            "Nombre", "Raza", "Sexo", "Condicion"ANSI_COLOR_RESET);
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

void atraparSeniales() {
    if (signal(SIGUSR1, signalHandler) == SIG_ERR)
        printf("No se pudo capturar la senal SIGUSR1\n");
    if (signal(SIGINT, signalHandler) == SIG_ERR)
        printf("No se pudo capturar la senal SIGUSR1\n");
}

//Validar entrada del usuario
//Aceptar key insensitive
//Extraer codigo a otros archivos