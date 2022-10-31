// #APL N. 3
// #EJERCICIO 2
// #INTEGRANTES:
//       #Carballo, Facundo Nicolas (DNI: 42774931)
//       #Garcia Burgio, Matias Nicolas (DNI: 42649117)
//       #Mottura, Agostina Micaela (DNI: 41898101)
//       #Povoli Olivera, Victor (DNI: 43103780)
//       #Szust, Ángel Elías (DNI: 43098495)

#include "monitoring.h"
#include <pwd.h>

void mostrarAyuda();
const char *getUserName();

int main(int argc, char* argv[]){
    if(argc == 1)
        printf("No se ingreso ningun archivo\n");
    else{
        if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
            mostrarAyuda();
            return 0;
        }
        else{
            struct stat sts;
            if (stat(argv[1], &sts) == -1) {
                fprintf(stderr, "Error stat: %s\n", strerror(errno));
                exit (1);
            } else if  (!S_ISDIR(sts.st_mode) ) {
                fprintf(stderr, "%s No es un directorio valido\n", argv[1]);
                exit (1);
            }
            FILE * file = fopen(LOG_FILE, "w");
            fclose(file);
            pthread_t hilo;
            tArgs *args = malloc(sizeof(tArgs));
            args->path = argv[1];
            List l;
            crearLista(&l);
            args->lista = &l;
            pthread_create(&hilo, NULL, mainThread, args);
            printf("Monitoreando %s y sus subdirectorios\n", argv[1]);
            printf("Presione enter para finalizar");
            
            getchar();
            pthread_cancel(hilo);

            while(!isEmpty(&l)){
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

void mostrarAyuda(){
    printf("Descripcion: \n");
    printf("\tEste programa monitorea un directorio y sus subdirectorios, y registra en un archivo de log los eventos que ocurren en el mismo.\n");
    printf("\tPor cada subdirectorio existente o creado en el directorio monitoreado, se crea un hilo que monitorea dicho subdirectorio.\n");
    printf("Ejemplo de ejecucion:\n");
    printf("\t./ej2 /home/%s/Desktop\n", getUserName());
    printf("Ejemlo de salida:\n");
    printf("\tMonitoreando /home/%s/Desktop y sus subdirectorios\n", getUserName());
    printf("\tPresione enter para finalizar\n");
    printf("*Se crea el archivo log.txt en el directorio actual\n");
    printf(" y el programa se queda ejecutando a la espera de cambios en los directorios monitoreados\n");
    printf("*Para finalizar el programa presione enter\n");
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