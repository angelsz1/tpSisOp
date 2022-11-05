#include "./monitoring.h"

void *mainThread(void *arg)
{
    char *path = ((tArgs *)arg)->path;
    if (path[strlen(path) - 1] == '/')
        *(path + strlen(path) - 1) = '\0';
    DIR *dir;
    char *newPath;
    struct dirent *ent;
    if ((dir = opendir(path)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0)
            {
                newPath = malloc(strlen(path) + strlen(ent->d_name) + 2);
                strcpy(newPath, path);
                strcat(newPath, "/");
                strcat(newPath, ent->d_name);
                pthread_t hilo;

                tArgs *args = malloc(sizeof(tArgs));
                args->path = newPath;
                args->lista = ((tArgs *)arg)->lista;

                pthread_t *pHilo = malloc(sizeof(pthread_t));
                pthread_create(&hilo, NULL, mainThread, args);
                *pHilo = hilo;
                tHilo *h = malloc(sizeof(tHilo));
                h->path = newPath;
                h->hilo = pHilo;
                add((((tArgs *)arg)->lista), h, sizeof(tHilo));
            }
        }
    }

    monitor(arg);
    return NULL;
}

void *monitor(void *arg)
{
    char *path = ((tArgs *)arg)->path;
    DIR *d;
    List *lista = (((tArgs *)arg)->lista);

    while ((d = opendir(path)) != NULL)
    {
        int length, i = 0;
        int fd;
        int wd;
        char buffer[BUF_LEN];
        fd = inotify_init();
        if (fd < 0)
        {
            perror("inotify_init");
        }
        wd = inotify_add_watch(fd, path, IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
        length = read(fd, buffer, BUF_LEN);
        if (length < 0)
        {
            perror("read");
        }
        while (i < length && (d = opendir(path)) != NULL)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len)
            {
                time_t t = time(NULL);
                struct tm tm = *localtime(&t);
                if (event->mask & IN_CREATE)
                {
                    if (event->mask & IN_ISDIR)
                    {
                        FILE *file = fopen(LOG_FILE, "a");
                        fprintf(file, "%d-%02d-%02d %02d:%02d:%02d ---> El directorio %s%s%s fue creado.\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, path, "/", event->name);
                        fclose(file);
                        char *newPath = malloc(strlen(path) + strlen(event->name) + 2);
                        strcpy(newPath, path);
                        strcat(newPath, "/");
                        strcat(newPath, event->name);
                        pthread_t hilo;
                        pthread_t *pHilo = malloc(sizeof(pthread_t));
                        tArgs *args = malloc(sizeof(tArgs));
                        args->path = newPath;
                        args->lista = ((tArgs *)arg)->lista;
                        pthread_create(&hilo, NULL, mainThread, args);
                        *pHilo = hilo;
                        tHilo *h = malloc(sizeof(tHilo));
                        h->path = newPath;
                        h->hilo = pHilo;
                        add(lista, h, sizeof(tHilo));
                    }
                    else
                    {
                        FILE *file = fopen(LOG_FILE, "a");
                        fprintf(file, "%d-%02d-%02d %02d:%02d:%02d ---> El archivo %s%s%s fue creado.\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, path, "/", event->name);
                        fclose(file);
                    }
                }
                else if (event->mask & IN_DELETE)
                {
                    if (event->mask & IN_ISDIR)
                    {
                        FILE *file = fopen(LOG_FILE, "a");
                        fprintf(file, "%d-%02d-%02d %02d:%02d:%02d ---> El directorio %s%s%s fue borrado.\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, path, "/", event->name);
                        fclose(file);
                        char *newPath = malloc(strlen(path) + strlen(event->name) + 2);
                        strcpy(newPath, path);
                        strcat(newPath, "/");
                        strcat(newPath, event->name);
                        tHilo *aux = malloc(sizeof(tHilo));
                        aux->path = newPath;
                        aux->hilo = NULL;
                        tHilo *h = (tHilo *)getByContent(lista, aux, sizeof(tHilo), cmp);

                        while (h != NULL)
                        {
                            pthread_cancel(*(h->hilo));
                            removeByContent(lista, aux, sizeof(tHilo), cmp);
                            h = (tHilo *)getByContent(lista, aux, sizeof(tHilo), cmp);
                        }
                    }
                    else
                    {
                        FILE *file = fopen(LOG_FILE, "a");
                        fprintf(file, "%d-%02d-%02d %02d:%02d:%02d ---> El archivo %s%s%s fue borrado.\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, path, "/", event->name);
                        fclose(file);
                    }
                }
                else if (event->mask & IN_MODIFY)
                {
                    if (event->mask & IN_ISDIR)
                    {
                        FILE *file = fopen(LOG_FILE, "a");
                        fprintf(file, "%d-%02d-%02d %02d:%02d:%02d ---> El directorio %s%s%s fue modificado.\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, path, "/", event->name);
                        fclose(file);
                    }
                    else
                    {
                        FILE *file = fopen(LOG_FILE, "a");
                        fprintf(file, "%d-%02d-%02d %02d:%02d:%02d ---> El archivo %s%s%s fue modificado.\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, path, "/", event->name);
                        fclose(file);
                    }
                }
                else if (event->mask & IN_MOVED_FROM)
                {
                    if (event->mask & IN_ISDIR)
                    {
                        FILE *file = fopen(LOG_FILE, "a");
                        fprintf(file, "%d-%02d-%02d %02d:%02d:%02d ---> El directorio %s%s%s fue movido a ", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, path, "/", event->name);
                        fclose(file);
                        char *newPath = malloc(strlen(path) + strlen(event->name) + 2);
                        strcpy(newPath, path);
                        strcat(newPath, "/");
                        strcat(newPath, event->name);
                        tHilo *aux = malloc(sizeof(tHilo));
                        aux->path = newPath;
                        aux->hilo = NULL;
                        tHilo *h = (tHilo *)getByContent(lista, aux, sizeof(tHilo), cmp);

                        while (h != NULL)
                        {
                            pthread_cancel(*(h->hilo));
                            removeByContent(lista, aux, sizeof(tHilo), cmp);
                            h = (tHilo *)getByContent(lista, aux, sizeof(tHilo), cmp);
                        }
                    }
                }
                else if (event->mask & IN_MOVED_TO)
                {
                    if (event->mask & IN_ISDIR)
                    {
                        FILE *file = fopen(LOG_FILE, "a");
                        fprintf(file, "%s%s%s\n", path, "/", event->name);
                        fclose(file);
                        char *newPath = malloc(strlen(path) + strlen(event->name) + 2);
                        strcpy(newPath, path);
                        strcat(newPath, "/");
                        strcat(newPath, event->name);
                        pthread_t hilo;
                        pthread_t *pHilo = malloc(sizeof(pthread_t));
                        tArgs *args = malloc(sizeof(tArgs));
                        args->path = newPath;
                        args->lista = ((tArgs *)arg)->lista;
                        pthread_create(&hilo, NULL, mainThread, args);
                        *pHilo = hilo;
                        tHilo *h = malloc(sizeof(tHilo));
                        h->path = newPath;
                        h->hilo = pHilo;
                        add(lista, h, sizeof(tHilo));
                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
        inotify_rm_watch(fd, wd);
        close(fd);
    }
    return NULL;
}

int cmp(void *a, void *b)
{
    tHilo *h1 = (tHilo *)a;
    tHilo *h2 = (tHilo *)b;
    return contains(h1->path, h2->path) == 1;
}

void print(void *a)
{
    tHilo *h = (tHilo *)a;
    printf("%s\t%d\n", h->path, *(int *)(h->hilo));
}

int contains(char *str, char *token)
{
    char *substr = strstr(str, token);
    return substr == NULL ? 0 : 1;
}
