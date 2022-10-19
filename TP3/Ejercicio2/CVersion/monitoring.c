#include "./monitoring.h"

void *mainThread(void* arg){
    char* path = (char*)arg;
    DIR *dir;
    char* newPath;
    struct dirent *ent;
    if ((dir = opendir (path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            //fprintf(file,"%s, %d\n", ent->d_name, ent->d_type);
            if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0){
                newPath = malloc(strlen(path) + strlen(ent->d_name) + 2);
                strcpy(newPath, path);
                strcat(newPath, "/");
                strcat(newPath, ent->d_name);
                pthread_t hilo;
                pthread_create(&hilo, NULL, mainThread, newPath);
                //fprintf(file,"Hilo creado para %s\n", newPath);
                //free(newPath);
            }
        }
    }
    printf("Monitor creado para %s\n", path);
    monitor(path);
    //free(newPath);
    //free(path);
    return NULL;

}

void *monitor(void* arg){
    char* path = (char*)arg;
    DIR *d;
    List lista;
    crearLista(&lista);
    while((d = opendir(path)) != NULL){
        //fprintf(file,"Path: %s\n", path);
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
                //fprintf(file,"hola");
                if (event->mask & IN_CREATE) {
                    if (event->mask & IN_ISDIR) {
                        FILE * file = fopen(LOG_FILE, "a");
                        fprintf(file,"The directory %s%s%s was created.\n", path, "/",event->name);
                        fclose(file);
                        char* newPath = malloc(strlen(path) + strlen(event->name) + 2);
                        strcpy(newPath, path);
                        strcat(newPath, "/");
                        strcat(newPath,event->name);
                        pthread_t hilo;
                        pthread_t* pHilo = malloc(sizeof(pthread_t));
                        pthread_create(&hilo, NULL, mainThread, newPath);
                        *pHilo = hilo;
                        tHilo* h = malloc(sizeof(tHilo));
                        h->path = newPath;
                        h->hilo = pHilo;
                        add(&lista, h, sizeof(tHilo));
                        printf("Hilo creado para %s\n", newPath);
                        //free(newPath);
                    }
                    else {
                        FILE * file = fopen(LOG_FILE, "a");
                        fprintf(file,"The file %s%s%s was created.\n",path, "/", event->name);
                        fclose(file);
                    }
                }
                else if (event->mask & IN_DELETE) {
                    if (event->mask & IN_ISDIR) {
                        FILE * file = fopen(LOG_FILE, "a");
                        fprintf(file,"The directory %s%s%s was deleted.\n",path, "/", event->name);
                        char* newPath = malloc(strlen(path) + strlen(event->name) + 2);
                        strcpy(newPath, path);
                        strcat(newPath, "/");
                        strcat(newPath,event->name);
                        tHilo* aux = malloc(sizeof(tHilo));
                        aux->path = newPath;
                        aux->hilo = NULL;
                        tHilo* h = (tHilo*)getByContent(&lista,aux, sizeof(tHilo), cmp);
                        if (h != NULL){
                            pthread_cancel(*(h->hilo));
                            removeByContent(&lista,aux, sizeof(tHilo), cmp);
                        }
                        fclose(file);
                    }
                    else {
                        FILE * file = fopen(LOG_FILE, "a");
                        fprintf(file,"The file %s%s%s was deleted.\n",path, "/", event->name);
                        fclose(file);
                    }
                }
                else if (event->mask & IN_MODIFY) {
                    if (event->mask & IN_ISDIR) {
                        FILE * file = fopen(LOG_FILE, "a");
                        fprintf(file,"The directory %s%s%s was modified.\n",path, "/", event->name);
                        fclose(file);
                    }
                    else {
                        FILE * file = fopen(LOG_FILE, "a");
                        fprintf(file,"The file %s%s%s was modified.\n",path, "/", event->name);
                        fclose(file);
                    }
                }
            }
            //fprintf(file,"hola");
            i += EVENT_SIZE + event->len;
        }
        //fprintf(file,"monitoring %s\n", path);
        inotify_rm_watch(fd, wd);
        close(fd);
    }
    return NULL;
}
