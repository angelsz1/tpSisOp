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
    char contenido[100];
} Respuesta;

void crearServidor();
int crearMemoriaCompartida();
int compararGato(const void* gato1, const void* gato2);
void mostrarGato(const void* gato);

int main()
{
    Lista listaGatos;
    crearLista(&listaGatos);

    sem_t* semComandos = sem_open("/comando", O_CREAT, 0666, 0);
    sem_t* semRespuesta = sem_open("/respuesta", O_CREAT, 0666, 0);
    if (semComandos == SEM_FAILED || semRespuesta == SEM_FAILED) {
        sem_close(semComandos);
        sem_close(semRespuesta);
        printf("Error creando el semaforo");
        exit(-1);
    }

    //crearServidor();
    int shmid = crearMemoriaCompartida();
    Pedido* pedido = (Pedido*)shmat(shmid,NULL,0);
    Respuesta* respuesta = (Respuesta*)shmat(shmid,NULL,0);
   
    int x=0;
    while (x<3)
    {
        x++;
        sem_wait(semComandos);

        if(strcmp("ALTA", pedido->accion) == 0) {
            Gato gato;
            strcpy(gato.nombre, pedido->nombre);
            strcpy(gato.raza, pedido->raza);
            gato.sexo = pedido->sexo;
            strcpy(gato.condicion, pedido->condicion);

            insertarEnListaOrdenada(&listaGatos, &gato, sizeof(Gato), compararGato);

            respuesta->status = 200;
            strcpy(respuesta->contenido, gato.nombre);
            strcat(respuesta->contenido, " ingresado correctamente");           
        }

        else if(strcmp("CONSULTA", pedido->accion) == 0) {
            Gato gato;
            strcpy(gato.nombre, pedido->nombre);
            
            int encontrado = buscarEnLista(&listaGatos, &gato, sizeof(Gato), compararGato);

            if(encontrado) {
                respuesta->status = 200;
                char mensaje[200];
                sprintf (mensaje, "Nombre: %s \nRaza: %s \nSexo: %c \nCondicion: %s\n", 
                gato.nombre, gato.raza, gato.sexo, gato.condicion);

                strcpy(respuesta->contenido, mensaje);
            } else {
                respuesta->status = 404;
                strcpy(respuesta->contenido, gato.nombre);
                strcat(respuesta->contenido, " no encontrado");
            }
        }

        else {
            char accion[9];
            strcpy(accion, pedido->accion);
            respuesta->status = 400;
            strcpy(respuesta->contenido, "BAD REQUEST: accion no valida ");
            strcat(respuesta->contenido, accion);
        }

        sem_post(semRespuesta);
    }

    shmdt(&pedido);
    shmdt(&respuesta);
    shmctl(shmid, IPC_RMID, NULL);

    sem_close(semComandos);
    sem_close(semRespuesta);

    sem_unlink("/comando");
    sem_unlink("/respuesta");
    mostrarLista(&listaGatos, mostrarGato);
    vaciarLista(&listaGatos);

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

int crearMemoriaCompartida() {
    char* clave = "../refugio";
    key_t key = ftok(clave, 4);
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);

    if (shmid == -1) {
        printf("Hubo un error al intentar abrir el área compartida de memoria.");
        exit(1);
    }

    return shmid;
}

int compararGato(const void* gato1, const void* gato2) {
    Gato g1 = *(Gato*)gato1;
    Gato g2 = *(Gato*)gato2;
    return strcmp(g1.nombre, g2.nombre);
}

void mostrarGato(const void* gato) {
    Gato g = *(Gato*)gato;
    printf("Nombre: %s\n", g.nombre);
}
 