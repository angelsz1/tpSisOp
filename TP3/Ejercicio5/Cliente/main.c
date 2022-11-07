// #APL N. 3
// #EJERCICIO 5
// #INTEGRANTES:
//       #Carballo, Facundo Nicolas (DNI: 42774931)
//       #Garcia Burgio, Matias Nicolas (DNI: 42649117)
//       #Mottura, Agostina Micaela (DNI: 41898101)
//       #Povoli Olivera, Victor (DNI: 43103780)
//       #Szust, Ángel Elías (DNI: 43098495)

#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h>

#include "socket.h"

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

char buf_tx[512];     
char buf_rx[512];
 
 
int main(int argc, char *argv[]) 
{ 
    int server_socket;
    struct sockaddr_in server_address; 
    
    if (argc > 1 && (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0))
    {
        ayuda();
        return 0;
    }

    if (argc < 4) 
    {
        puts(COLOR_RED"Argumentos mal pasados"COLOR_RESET);
        ayuda();
        return 1;
    }

    server_socket = socket_create();
    server_address = socket_config(argv[1], argv[2]);

    socket_connect(server_socket, server_address);    
      
    // Copiamos en el buffer el mensaje a enviar
    strcpy(buf_tx, argv[3]);
    strcat(buf_tx, " ");
    
    int i = 4;
    while(i < argc) {
        strcat(buf_tx, argv[i]);
        i++;
        if (i != argc) strcat(buf_tx, " ");
    }
    

    // Escribimos el mensaje
    write(server_socket, buf_tx, sizeof(buf_tx));     
    
    // Leemos la respuesta del servidor
    read(server_socket, buf_rx, sizeof(buf_rx));


    printf("Respuesta del servidor: \n%s\n", buf_rx);
   
    close(server_socket); 
} 