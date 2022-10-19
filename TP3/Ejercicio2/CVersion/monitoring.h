#ifndef header_h
#define header_h

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))
#define LOG_FILE "log.txt"


#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/inotify.h>
#include "Lista/lista.h"

typedef struct{
    char* path;
    pthread_t* hilo;
}tHilo;


void *monitor(void* arg);
void *mainThread(void* arg);

#endif /* header_h */
