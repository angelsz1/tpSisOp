#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include "../global.h"

char* parsearCampo(char* texto, char* campo, char* nombreCampo, int maxCaracteres);
int parsearPedido(char* texto, Pedido* pedido);
int crearMemoriaCompartida();
void toUpper(char* s);

int main()
{
    sem_t* semComando = sem_open("/comando", O_CREAT, 0666, 0);
    sem_t* semRespuesta = sem_open("/respuesta", O_CREAT, 0666, 0);
    if (semComando == SEM_FAILED || semRespuesta == SEM_FAILED) {
        sem_close(semComando);
        sem_close(semRespuesta);
        printf(COLOR_RED"Error abriendo los semaforos"COLOR_RESET);
        exit(-1);
    }

    int shmid = crearMemoriaCompartida();
    Pedido* pedido = (Pedido*)shmat(shmid,NULL,0);
    Respuesta* respuesta = (Respuesta*)shmat(shmid,NULL,0);

    char texto[200];

    printf("¿Que accion desea realizar?\n");
    printf("ALTA [nombre] [raza] [M/F] [CA/SC]\n");
    printf("BAJA [nombre]\n");
    printf("CONSULTA [?nombre]\n");
    printf("SALIR\n\n");
    fgets(texto, 200, stdin);
    
    int resultado = parsearPedido(&texto, pedido);

    while (strcmp(pedido->accion, "SALIR") != 0) {
        if(resultado == TODO_OK) {
            sem_post(semComando);
            sem_wait(semRespuesta);

            if(respuesta->status >= 200 && respuesta->status < 300) {
                printf("%s\n", respuesta->contenido);
            } else {
                printf(COLOR_RED"Status code: %d\n", respuesta->status);
                printf("%s\n" COLOR_RESET, respuesta->contenido);
            }
            printf("\n¿Que accion desea realizar?\n");
            fgets(texto, 200, stdin);
        } else {
            printf("Por favor, intente nuevamente\n");
            fgets(texto, 200, stdin);
        }
        
        int resultado = parsearPedido(&texto, pedido);
    }

    shmdt(&pedido);
    shmdt(&respuesta);
    shmctl(shmid, IPC_RMID, NULL);

    sem_close(semComando);
    sem_close(semRespuesta);

    return 0;
}


char* parsearCampo(char* texto, char* campo, char* nombreCampo, int maxCaracteres) {
    char caracteres[maxCaracteres];
    int i = 0;

    while(*texto != ' ' && *texto != '\0' && *texto != '\n') {
        caracteres[i] = *texto;
        i++;
        texto++;
    }

    if(*texto != '\0')
        texto++;

    caracteres[i] = '\0';

    if(i<=maxCaracteres) {
        strcpy(campo, caracteres);
    }  
    else {
        if(maxCaracteres > 1) {
            printf(COLOR_YELLOW"El campo %s no puede contener mas de %d caracteres\n"COLOR_RESET, nombreCampo, maxCaracteres);
        } else 
            printf(COLOR_YELLOW"El campo %s no pueden contener mas de %d caracter\n"COLOR_RESET, nombreCampo, maxCaracteres);
        strcpy(campo, "?");
    }

    return texto;
}

int parsearPedido(char* texto, Pedido* pedido) {
    texto = parsearCampo(texto, pedido->accion, "accion", 25);
    texto = parsearCampo(texto, pedido->nombre, "nombre", 25);
    texto = parsearCampo(texto, pedido->raza, "raza", 25);
    texto = parsearCampo(texto, &(pedido->sexo), "sexo", 1);
    texto = parsearCampo(texto, pedido->condicion, "condicion", 2);

    toUpper(pedido->accion);

    if(strcmp(pedido->accion, "?") == 0 || 
        strcmp(pedido->nombre, "?") == 0 || 
        strcmp(pedido->raza, "?") == 0 || 
        strcmp(&(pedido->sexo), "?") == 0 ||
        strcmp(pedido->condicion, "?") == 0)
        return 0;
    
    return 1;
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

void toUpper(char* s) {
    while(*s != '\0') {
        if(*s >= 'a' && *s <= 'z') {
            *s = *s -32;
        }
      s++;
    }
}