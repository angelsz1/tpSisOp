#ifndef GLOBAL_H
#define GLOBAL_H
#include <stddef.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

typedef struct {
    char accion[15]; 
    char nombre[20];
    char raza[20];
    char sexo[10];
    char condicion[10];
} Pedido;

typedef struct {
    char nombre[20];
    char raza[20];
    char sexo;
    char condicion[3];
} Gato;

typedef struct {
    int status; 
    char contenido[2000];
} Respuesta;

#endif //GLOBAL_H
