#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include "../global.h"

int crearMemoriaCompartida();
void toUpper(char* s);
void ayuda();
void validarParametros(int cantidadParametros, char* parametros[]);

int main(int argc, char* argv[])
{
    validarParametros(argc, argv);

    sem_t* semComando = sem_open("/comando", 0666, 0);
    sem_t* semRespuesta = sem_open("/respuesta", 0666, 0);
    if (semComando == SEM_FAILED || semRespuesta == SEM_FAILED) {
        sem_close(semComando);
        sem_close(semRespuesta);
        printf(RED"STATUS CODE: 503\n");
        printf(RED"El server no se encuentra activo\n"RESET);
        exit(-1);
    }

    int shmid = crearMemoriaCompartida();
    char* pedido = (char*)shmat(shmid,NULL,0);
    Respuesta* respuesta = (Respuesta*)shmat(shmid,NULL,0);

    printf("¿Que accion desea realizar?\n");
    printf("ALTA [nombre] [raza] [M/F] [CA/SC]\n");
    printf("BAJA [nombre]\n");
    printf("CONSULTA [?nombre]\n");
    printf("SALIR\n\n");
    fgets(pedido, 200, stdin);
    toUpper(pedido);

    while (strcmp(pedido, "SALIR") != 0) {
        sem_post(semComando);
        sem_wait(semRespuesta);

        if(respuesta->status >= 200 && respuesta->status < 300) {
            printf("%s\n", respuesta->contenido);
        } else {
            printf(RED"Status code: %d\n", respuesta->status);
            printf("%s\n" RESET, respuesta->contenido);
        }
        printf("\n¿Que accion desea realizar?\n");
        fgets(pedido, 200, stdin);
        toUpper(pedido);
    }

    shmdt(&pedido);
    shmdt(&respuesta);
    shmctl(shmid, IPC_RMID, NULL);

    sem_close(semComando);
    sem_close(semRespuesta);

    return 0;
}

int crearMemoriaCompartida() {
    char* clave = "../refugio";
    key_t key = ftok(clave, 4);
    int shmid = shmget(key, sizeof(int), 0666);

    if (shmid == -1) {
        printf(YELLOW"Hubo un error al intentar abrir el área de memoria compartida.\n"YELLOW);
        exit(1);
    }

    return shmid;
}

void toUpper(char* s) {
    while(*s != '\0' && *s !='\n') {
        if(*s >= 'a' && *s <= 'z') {
            *s = *s -32;
        }
      s++;
    }
    if(*s == '\n') {
        *s = '\0';
    }
}

void validarParametros(int cantidadParametros, char* parametros[]) {
    if (cantidadParametros > 2) {
        printf("./cliente no soporta %d parametros", cantidadParametros);
        exit(0);
    }

    if(cantidadParametros == 2) {
        if (strcmp(parametros[1],"-help") == 0 || strcmp(parametros[1], "-h") == 0 || strcmp(parametros[1], "--help") == 0) {
            ayuda();
            exit(1);
        } else {
            printf(YELLOW"Argumento invalido: %s\n"RESET, parametros[1]);
            exit(0);
        }
    }
}

void ayuda() {
    puts(YELLOW"\nDESCRIPCION"RESET);
    puts("Con este script podras solicitar informacion al servidor del refugio de gatos\n");
    puts(YELLOW"USO"RESET);
    puts("Para solicitar informacion usted debera invocar al script de la siguiente forma:");
    puts("./Cliente\n");
    puts("El cliente acepta las siguientes acciones:\n");
    puts(CYAN"- ALTA:"RESET);
    printf("\t● ALTA ");
    printf(GREEN"[nombre] [raza] [sexo] [condicion]\n"RESET);
    puts(CYAN"- BAJA:"RESET);
    printf("\t● BAJA ");
    printf(GREEN"[nombre]\n"RESET);
    puts(CYAN"- CONSULTA:"RESET);
    puts("\t● CONSULTA");
    printf("\t● CONSULTA ");
    printf(GREEN"[nombre]\n"RESET);
    puts(CYAN"- SALIR\n"RESET);
    puts(YELLOW"PARAMETROS"RESET);
    printf(GREEN"[nombre]\t"RESET);
    printf("Nombre del gato (MAXIMO 25 CARACTERES)\n");
    printf(GREEN"[raza]\t\t"RESET);
    printf("Raza del gato (MAXIMO 25 CARACTERES)\n");
    printf(GREEN"[sexo]\t\t"RESET);
    printf("Sexo del gato (M o F) (SE DEBE COLOCAR UN SOLO CARACTER)\n");
    printf(GREEN"[condicion]\t"RESET);
    printf("Castrado (CA) o Sin Castrar (SC) (SE DEBEN COLOCAR SOLO 2 CARACTERES)\n\n");
    puts(YELLOW"ACLARACIONES:"RESET);
    puts("La consulta sin parametros te mostrara el registro completo de gatos que se encuentran en el refugio");
    puts("La consulta que lleva el parametro [nombre] te mostrara la informacion del gato buscado si es que existe");    
    puts("------------------------------------------------------------------------");
}