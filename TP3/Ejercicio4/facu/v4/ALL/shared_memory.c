#include "shared_memory.h"

char *shared_memory_create(int *id)
{
    char *memComp;
    char* clave = "../refugio";

    key_t key = ftok(clave, 4);

    *id = shmget(key, sizeof(int), IPC_CREAT | 0666);

    if (*id == -1)
    {
        puts("Hubo un error al intentar crear el área compartida de memoria (shmget).");
        exit(1);
    }

    memComp = (char*)shmat(*id, NULL, 0);

    if (memComp == (void *)-1)
    {
        puts("Hubo un error al intentar crear el área compartida de memoria (shmat).");
        exit(1);
    }

    return memComp;
}

void shared_memory_destroy(char * shm, int id)
{
    if (shmdt(shm) == -1 || shmctl(id, IPC_RMID, NULL) == -1)
    {
        puts("Hubo un error al intentar destruir el área compartida de memoria.");
        exit(1);
    }
}

void shared_memory_write(const char *msg, char *shm, sem_t *are_query,
                         sem_t *mutex_memory, sem_t *are_response)
{
    sem_wait(are_query);
    sem_wait(mutex_memory);
    strcpy(shm, msg);
    sem_post(mutex_memory);
    sem_post(are_response);
}

char *shared_memory_open(int *id)
{
    
}