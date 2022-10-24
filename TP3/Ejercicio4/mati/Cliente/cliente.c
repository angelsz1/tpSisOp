#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include "../global.h"

char* parsearCampo(char* texto, char* campo) {
    char caracteres[20];
    int i = 0;

    while(*texto != ' ' && *texto != '\0' && *texto != '\n') {
        caracteres[i] = *texto;
        i++;
        texto++;
    }

    if(*texto != '\0')
        texto++;

    caracteres[i] = '\0';
    strcpy(campo, caracteres);

    return texto;
}

void parsearPedido(char* texto, Pedido* pedido) {
    texto = parsearCampo(texto, pedido->accion);
    texto = parsearCampo(texto, pedido->nombre);
    texto = parsearCampo(texto, pedido->raza);
    texto = parsearCampo(texto, &(pedido->sexo));
    texto = parsearCampo(texto, pedido->condicion);
}

int main()
{
    sem_t* semComando = sem_open("/comando", O_CREAT, 0666, 0);
    sem_t* semRespuesta = sem_open("/respuesta", O_CREAT, 0666, 0);

    char* clave = "../refugio";
    key_t key = ftok(clave, 4);
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("Hubo un error al intentar abrir el área compartida de memoria.");
        exit(1);
    }

    Pedido* pedido = (Pedido*)shmat(shmid,NULL,0);
    Respuesta* respuesta = (Respuesta*)shmat(shmid,NULL,0);

    char texto[200];

    printf("¿Que accion desea realizar?\n");
    printf("ALTA [nombre] [raza] [sexo] [condicion]\n");
    printf("BAJA [nombre]\n");
    printf("CONSULTA [?nombre]\n\n");
    printf("SALIR\n");
    fgets(texto, 200, stdin);
    printf("\n");
    parsearPedido(&texto, pedido);

    while (strcmp(pedido->accion, "SALIR") != 0) {

        sem_post(semComando);
        sem_wait(semRespuesta);

        if(respuesta->status >= 200 && respuesta->status < 300) {
            printf("%s\n", respuesta->contenido);
        } else {
            printf(ANSI_COLOR_RED"Status code: %d\n", respuesta->status);
            printf("Error. %s\n" ANSI_COLOR_RESET, respuesta->contenido);
        }

        printf("\n¿Que accion desea realizar?\n");
        fgets(texto, 200, stdin);
        parsearPedido(&texto, pedido);
    }

    shmdt(&pedido);
    shmdt(&respuesta);
    shmctl(shmid, IPC_RMID, NULL);

    sem_close(semComando);
    sem_close(semRespuesta);

    return 0;
}

