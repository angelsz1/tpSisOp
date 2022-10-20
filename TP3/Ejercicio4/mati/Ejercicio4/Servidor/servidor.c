#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void crearServidor();

int main()
{

    crearServidor();

    char* clave = "../refugio";
    key_t key = ftok(clave, 4);

    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);

    int *areaCompartida = (int*)shmat(shmid,NULL,0);

    *areaCompartida = 3;

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