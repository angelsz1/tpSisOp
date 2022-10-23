#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>

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

void crearServidor();
int crearMemoriaCompartida();

int main()
{
    
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
   
    sem_wait(semComandos);

    if(strcmp("ALTA", pedido->accion) == 0) {
        FILE* gatosFile = fopen("gatos.txt", "wb");
        Gato gato;
        strcpy(gato.nombre, pedido->nombre);
        strcpy(gato.raza, pedido->raza);
        gato.sexo = pedido->sexo;
        strcpy(gato.condicion, pedido->condicion);
        
        fwrite(&gato, sizeof(Gato), 1, gatosFile);
        fclose(gatosFile);
    }

    if(strcmp("CONSULTA", pedido->accion) == 0) {
        FILE* gatosFile = fopen("gatos.txt", "rb");
        Gato gato;
        fread(&gato, sizeof(Gato), 1, gatosFile);
        printf("NOMBRE: %s\nRAZA: %s", gato.nombre, gato.raza);
        fclose(gatosFile);
    }

    shmdt(&pedido);
    shmctl(shmid, IPC_RMID, NULL);

    sem_close(semComandos);
    sem_close(semRespuesta);

    sem_unlink("/comando");
    sem_unlink("/respuesta");

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