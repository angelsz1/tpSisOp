// #include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "cat.h"
#include "query.h"
#include "server.h"

void ayuda() {
    puts("--------------------------- AYUDA DEL SCRIPT ---------------------------");
    puts("Con este script crearas un servidor que estara escuchando peticiones de un cliente");
    puts("El cliente tiene 3 tipos de peticiones para hacerle a este servidor:");
    puts("- ALTA:");
    puts("       - Persiste la informacion de un gato en logs.txt si es que este no existe");
    puts("- BAJA:");
    puts("       - Elimina la informacion de un gato en logs.txt si es que este existe");
    puts("- CONSULTA:");
    puts("       - Devuelve la informacion de los gatos que se encuentran en logs.txt");
    puts("       - Devuelve la informacion de un gato en especifico si es que este existe");
    puts("IMPORTANTE:");
    puts("       - De querer finalizar el proceso tendran que hacerlo atraves de la senial SIGUSR1 (haciendole un kill [pid])");    
    puts("------------------------------------------------------------------------");

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
