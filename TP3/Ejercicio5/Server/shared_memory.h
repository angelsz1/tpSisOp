#ifndef SHARED_MEMORY_H_INCLUDED
#define SHARED_MEMORY_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>

#include "query.h"

#define SIZE_SHM 10240 // 10KB

char* shared_memory_create(int*);
void shared_memory_destroy(char*, int id);
void shared_memory_write(const char*, char*, sem_t*, 
                        sem_t*, sem_t*);
char* shared_memory_open(int* id);

#endif // SHARED_MEMORY_H_INCLUDED