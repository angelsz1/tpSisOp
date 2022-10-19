
#include "monitoring.h"


int main(int argc, char* argv[]){
    if(argc == 1)
        printf("No se ingreso ningun archivo\n");
    else{
        if(strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
            printf("Uso: %s [archivo]\n", argv[0]);
            printf("Si no sabes usarlo sos un pelotudo\n");
            return 0;
        }
        else{
            FILE * file = fopen(LOG_FILE, "w");
            fclose(file);
            pthread_t hilo;
            pthread_create(&hilo, NULL, mainThread, argv[1]);
            //enter to close thread
            getchar();

            printf("Cerrando programa\n");
            pthread_cancel(hilo);
            pthread_join(hilo, NULL);
        }
    }
    return 0;
}

