#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>

#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RESET "\x1b[0m"

void ayuda() {
    puts(COLOR_GREEN"---------------------------------------- AYUDA DEL SCRIPT ----------------------------------------"COLOR_RESET);
    puts("Con este script podras solicitar informacion al servidor del refugio de gatos");
    puts("Para solicitar informacion usted debera invocar al script de la siguiente forma:");
    puts(COLOR_YELLOW"- ALTA:"COLOR_RESET);
    puts("       ./main [Direccion IP] [Puerto] ALTA [nombre] [raza] [SEXO] [Condicion Sexual]");
    puts(COLOR_YELLOW"- BAJA:"COLOR_RESET);
    puts("       ./main [Direccion IP] [Puerto] BAJA [nombre]");
    puts(COLOR_YELLOW"- CONSULTA:"COLOR_RESET);
    puts("       ./main [Direccion IP] [Puerto] CONSULTA");
    puts("       ./main [Direccion IP] [Puerto] CONSULTA [nombre]");
    puts(COLOR_YELLOW"Referencias:"COLOR_RESET);
    puts("[Direccion IP] Direccion IP donde se encuentra el servidor");
    puts("[Puerto] Puerto en el que se encuentra el servidor");
    puts("[nombre] Nombre del gato (MAXIMO 50 CARACTERES)");
    puts("[raza]   Raza del gato (MAXIMO 50 CARACTERES)");
    puts("[SEXO]   Sexo del gato (M o F) (SE DEBE COLOCAR UN SOLO CARACTER)");
    puts("[Condicion Sexual] Castrado (CA) o Sin Castrar (SC) (SE DEBEN COLOCAR SOLO 2 CARACTERES)");
    puts("Aclaraciones:");
    puts("La consulta sin parametros te mostrara el registro completo de gatos que se encuentran en el refugio");
    puts("La consulta que lleva el parametro [nombre] te mostrara la informacion del gato buscado si es que existe");
    puts(COLOR_YELLOW"IMPORTANTE:"COLOR_RESET);
    puts("Cada parametro debera estar separado por un espacio en blaco.");    
    puts(COLOR_GREEN"--------------------------------------------------------------------------------------------------"COLOR_RESET);

}

char * abrirSHM(int *id) {
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

int main(int argc, char* argv[]) {

    
    if (argc < 2 || argc > 6) {
        puts(COLOR_RED"Has pasado mal los argumentos"COLOR_RESET);
        ayuda();
        return 0;
    }

    if (strcmp(argv[1],"-help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
		ayuda();
        return 0;
	}


    char query[512];
    strcpy(query, argv[1]);
    strcat(query, " ");
    
    int i = 2;
    while(i < argc) {
        strcat(query, argv[i]);
        i++;
        if (i != argc) strcat(query, " ");
    }

    
    // Hay que escribir en memoria compartida toda la linea de texto
    int shmid;
    char * memComp = abrirSHM(&shmid);

    // Obtenemos los semaforos
    sem_t* mutex_memory = sem_open("mutex", 0, 0666, 1);
    sem_t* are_query = sem_open("/are_query", 0, 0666, 0);
    sem_t* are_response = sem_open("/are_response", 0, 0666, 0);

    if (mutex_memory == SEM_FAILED || are_query == SEM_FAILED || are_response == SEM_FAILED) {
        puts("Error al abrir el semaforo");
        exit(1);
    }
    char res[512];

    sem_wait(mutex_memory); // P
    strcpy(memComp, query);
    sem_post(are_query); // V
    sem_post(mutex_memory); // V

    sem_wait(are_response); // P

    sem_wait(mutex_memory); // P
    strcpy(res, memComp);
    printf("Respuesta:\n%s\n", res);
    sem_post(mutex_memory); // V

    return 0;
}