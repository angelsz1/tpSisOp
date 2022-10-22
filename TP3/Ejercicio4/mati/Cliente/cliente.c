#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>

int main()
{
    char* clave = "../refugio";
    key_t key = ftok(clave, 4);
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        printf("Hubo un error al intentar abrir el área compartida de memoria.");
        exit(1);
    }
    char* areaCompartida = (char*)shmat(shmid,NULL,0);

    sem_t* semComandos = sem_open("/comando", O_CREAT, 0666, 0);

    printf("Ecribi algo: ");
    fgets(areaCompartida, 20, stdin);
    
    sem_post(semComandos);

    //shmdt(&areaCompartida);
    //shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
