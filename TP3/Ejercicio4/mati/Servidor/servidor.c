#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>

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
    char* areaCompartida = (char*)shmat(shmid,NULL,0);
   

    printf("Ejecuta\n");
    sem_wait(semComandos);

    printf("El cliente dice %s", areaCompartida);

    shmdt(&areaCompartida);
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