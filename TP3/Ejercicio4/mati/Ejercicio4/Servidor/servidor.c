#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main()
{
    char* clave = "../refugio";
    key_t key = ftok(clave, 4);

    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);

    int *areaCompartida = (int*)shmat(shmid,NULL,0);

    *areaCompartida = 3;

    return 0;
}
