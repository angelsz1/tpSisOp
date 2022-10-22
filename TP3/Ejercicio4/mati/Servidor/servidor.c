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
    printf("Llega hasta aca");
    //crearServidor();
    int shmid = crearMemoriaCompartida();
    char* areaCompartida = (char*)shmat(shmid,NULL,0);

    sem_t *semComandos = sem_open("/comando", O_CREAT, 0666, 0);
    sem_t *semRespuesta = sem_open("/respuesta", O_CREAT, 0666, 0);

    //Crear semaforo para area compartida
    while (1) {
        printf("Llega hasta aca");
        sem_wait(semComandos);
        printf("No Llega hasta aca");
        //P area compartida
        if (strcmp(areaCompartida, "Alta") == 0) {
            areaCompartida = "";
            sem_post(semRespuesta);
        };

        //V area compartida

        //P area compartida
        if (strcmp(areaCompartida, "Baja") == 0) {
            areaCompartida = "";
            sem_post(semRespuesta);
        };
        //V area compartida
        
        //P area compartida
        if (strcmp(areaCompartida, "Consulta") == 0) {
            areaCompartida = "";
            sem_post(semRespuesta);
        };
        //V area compartida
    }

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
    printf("KEY: %d", key);
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);

    if (shmid == -1) {
        printf("Hubo un error al intentar abrir el área compartida de memoria.");
        exit(1);
    }

    return shmid;
}