// #include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cat.h"
#include "query.h"
#include "server.h"

void ayuda()
{
    puts(COLOR_GREEN"-------------------------------------------------- AYUDA DEL SCRIPT --------------------------------------------------"COLOR_RESET);
    puts("Con este script crearas un servidor que estara escuchando peticiones de un cliente");
    puts("El cliente tiene 3 tipos de peticiones para hacerle a este servidor:");
    puts(COLOR_YELLOW"- ALTA:"COLOR_RESET);
    puts("       - Persiste la informacion de un gato en logs.txt (Si el gato ya existe te informara por pantalla)");
    puts(COLOR_YELLOW"- BAJA:"COLOR_RESET);
    puts("       - Elimina la informacion de un gato en logs.txt (Si el gato no existiese te informara por pantalla)");
    puts(COLOR_YELLOW"- CONSULTA:"COLOR_RESET);
    puts("       - Devuelve la informacion de los gatos que se encuentran en logs.txt");
    puts("       - Devuelve la informacion de un gato en especifico (Si el gato no existiese te informara por pantalla)");
    puts(COLOR_YELLOW "EJECUCION:" COLOR_RESET);
    puts("Para ejecutarlo de manera correcta deberas ejecutar el siguiente comando:");
    puts("./main");
    puts(COLOR_YELLOW"IMPORTANTE:"COLOR_RESET);
    puts("       - De querer finalizar el proceso tendran que hacerlo atraves de la senial SIGUSR1 (haciendole un kill -SIGUSR1 [PID])");
    puts(COLOR_GREEN"----------------------------------------------------------------------------------------------------------------------"COLOR_RESET);
}


int main(int argc, char *argv[])
{
    
    if (argc > 1 && (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        ayuda();
        return 0;
    }

    server_create();
    return 0;
}
