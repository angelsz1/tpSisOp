#include <stdio.h>
#include <pthread.h>
#include <sys/inotify.h>
#include "header.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>


#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024 * (EVENT_SIZE + 16))


void *monitor(void* arg){
    DIR *d;
    char *path = (char*)arg;
    while((d = opendir(path)) != NULL){
        //printf("Path: %s\n", path);
        int length, i = 0;
        int fd;
        int wd;
        char buffer[BUF_LEN];
        fd = inotify_init();
        if (fd < 0) {
            perror("inotify_init");
        }
        wd = inotify_add_watch(fd, path, IN_CREATE | IN_DELETE | IN_MODIFY);
        length = read(fd, buffer, BUF_LEN);
        if (length < 0) {
            perror("read");
        }
        while (i < length && (d = opendir(path)) != NULL) {
            struct inotify_event *event = (struct inotify_event *) &buffer[i];
            if (event->len) {
                if (event->mask & IN_CREATE) {
                    if (event->mask & IN_ISDIR) {
                        printf("The directory %s was created.\n", event->name);
                    }
                    else {
                        printf("The file %s was created.\n", event->name);
                    }
                }
                else if (event->mask & IN_DELETE) {
                    if (event->mask & IN_ISDIR) {
                        printf("The directory %s was deleted.\n", event->name);
                    }
                    else {
                        printf("The file %s was deleted.\n", event->name);
                    }
                }
                else if (event->mask & IN_MODIFY) {
                    if (event->mask & IN_ISDIR) {
                        printf("The directory %s was modified.\n", event->name);
                    }
                    else {
                        printf("The file %s was modified.\n", event->name);
                    }
                }
            }
            //printf("hola");
            i += EVENT_SIZE + event->len;
        }
        //printf("monitoring %s\n", path);
        inotify_rm_watch(fd, wd);
        close(fd);
    }
    return NULL;
}

int main(int argc, char* argv[]){
    if(argc == 1)
        printf("No se ingreso ningun archivo\n");
    else{
        if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
            printf("Uso: %s [archivo]\n", argv[0]);
            return 0;
        }
        else{
            pthread_t hilo;
            pthread_create(&hilo, NULL, monitor, argv[1]);
            printf("Esta andando el hilo\n");
            pthread_join(hilo, NULL);
            printf("Termino el hilo\n");
        }
    }
    return 0;
}

