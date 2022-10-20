#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

void crearServidor();
int crearMemoriaCompartida();

int main()
{
    crearServidor();
    int shmid = crearMemoriaCompartida();

    char* areaCompartida = (char*)shmat(shmid,NULL,0);

    while (1) {
        if (strcmp(areaCompartida, "Alta") == 0) {
            printf("Hacemos alta\n");
            areaCompartida = "";
        };

        if (strcmp(areaCompartida, "Baja") == 0) {
            printf("Hacemos baja\n");
            areaCompartida = "";
        };

        if (strcmp(areaCompartida, "Consulta") == 0) {
            printf("Hacemos consulta\n");
            areaCompartida = "";
        };
    }
    
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