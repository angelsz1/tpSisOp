// #APL N. 3
// #EJERCICIO 2
// #INTEGRANTES:
//       #Carballo, Facundo Nicolas (DNI: 42774931)
//       #Garcia Burgio, Matias Nicolas (DNI: 42649117)
//       #Mottura, Agostina Micaela (DNI: 41898101)
//       #Povoli Olivera, Victor (DNI: 43103780)
//       #Szust, Ángel Elías (DNI: 43098495)

#include "monitoring.h"
#include <signal.h>
#include <pwd.h>
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

void mostrarAyuda();
const char *getUserName();
void sigint_handler(int sig);

List l;

int main(int argc, char *argv[])
{
    if (argc == 1)
        printf(RED "ERROR : No se ingreso ningun path a monitorear.\n" RESET);
    else
    {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        {
            mostrarAyuda();
            return 0;
        }
        else
        {
            struct stat sts;
            if (stat(argv[1], &sts) == -1)
            {
                fprintf(stderr, RED "Error: %s\n" RESET, strerror(errno));
                exit(1);
            }
            else if (!S_ISDIR(sts.st_mode))
            {
                fprintf(stderr, RED "Error : %s no es un directorio valido\n" RESET, argv[1]);
                exit(1);
            }
            FILE *file = fopen(LOG_FILE, "w");
            fclose(file);
            pthread_t hilo;
            tArgs *args = malloc(sizeof(tArgs));
            args->path = argv[1];
            crearLista(&l);
            args->lista = &l;
            pthread_create(&hilo, NULL, mainThread, args);

            char *pathPrincipal = (char *)malloc(strlen(argv[1]) + 1);
            pthread_t *pHilo = malloc(sizeof(pthread_t));
            *pHilo = hilo;
            strcpy(pathPrincipal, argv[1]);
            tHilo *h = malloc(sizeof(tHilo));
            h->path = pathPrincipal;
            h->hilo = pHilo;
            add(&l, h, sizeof(tHilo));

            printf(GREEN "Monitoreando %s y sus subdirectorios\n" RESET, argv[1]);

            struct sigaction sa;
            memset(&sa, 0, sizeof(sa));
            sa.sa_handler = &sigint_handler;
            sigaction(SIGINT, &sa, NULL);

            // printf("Presione enter para finalizar\n");

            // getchar();
            // pthread_cancel(hilo);
            // loop to see if the path in argv[1] is still a dir
            while (1)
            {
                if (stat(argv[1], &sts) == -1 || !S_ISDIR(sts.st_mode))
                {
                    FILE *file = fopen(LOG_FILE, "a");
                    if (!file)
                    {
                        fclose(file);
                        exit(1);
                    }
                    time_t t = time(NULL);
                    struct tm tm = *localtime(&t);

                    fprintf(file, "%d-%02d-%02d %02d:%02d:%02d ---> El directorio raiz (%s) fue borrado.\n", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec, argv[1]);
                    fclose(file);

                    printf(RED "Monitoreo finalizado (Directorio raiz eliminado)\n" RESET);

                    break;
                }
                sleep(1);
            }

            pthread_cancel(hilo);
            while (!isEmpty(&l))
            {
                tHilo *h = removeFirstFromList(&l);
                pthread_cancel(*h->hilo);
                free(h->path);
                free(h->hilo);
                free(h);
            }
        }
    }
    return 0;
}

void mostrarAyuda()
{
    printf("Descripcion: \n");
    printf("\tEste programa monitorea un directorio y sus subdirectorios, y registra en un archivo de log los eventos que ocurren en el mismo.\n");
    printf("\tPor cada subdirectorio existente o creado en el directorio monitoreado, se crea un hilo que monitorea dicho subdirectorio.\n");
    printf("Ejemplo de ejecucion:\n");
    printf("\t./ej2 /home/%s/Desktop\n", getUserName());
    printf("Ejemplo de salida:\n");
    printf("\tMonitoreando /home/%s/Desktop y sus subdirectorios\n", getUserName());
    printf("\tPresione enter para finalizar\n");
    printf("*Se crea el archivo log.txt en el directorio actual\n");
    printf(" y el programa se queda ejecutando a la espera de cambios en los directorios monitoreados\n");
    printf("*Para finalizar el programa presione ctrl-c\n");
    printf("*Se finalizan todos los hilos y se cierra el programa\n");
}

const char *getUserName()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw)
    {
        return pw->pw_name;
    }

    return "";
}

void sigint_handler(int sig)
{

    sigaction(SIGINT, NULL, NULL);
    while (!isEmpty(&l))
    {
        tHilo *h = removeFirstFromList(&l);
        pthread_cancel(*h->hilo);
        free(h->path);
        free(h->hilo);
        free(h);
    }
    printf(GREEN "\nMonitoreo finalizado (Ctrl+C)\n" RESET);
    exit(0);
}