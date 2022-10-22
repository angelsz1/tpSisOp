#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>


#include "shared_memory.h"
#include "const.h"



void server_create();
void server_wait_query(const char*);
void server_read_query(const char*, const char*,
                        sem_t*, sem_t*,
                        sem_t*, FILE*);
char server_get_query(const char*);
int server_up(const char*, FILE* arch, sem_t*, sem_t*, sem_t*);
int server_down(const char*, FILE* arch, sem_t*, sem_t*, sem_t*);
int server_query(const char*, FILE* arch, sem_t*, sem_t*, sem_t*);


#endif // SERVER_H_INCLUDED