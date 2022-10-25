#ifndef GLOBAL_H
#define GLOBAL_H
#include <stddef.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_RESET   "\x1b[0m"

#define TODO_OK 1
#define ELIMINADO 1
#define NOT_FOUND 0
#define DUPLICADO 2

typedef struct {
    char accion[25]; 
    char nombre[25];
    char raza[25];
    char sexo[2];
    char condicion[3];
} Pedido;

typedef struct {
    char nombre[25];
    char raza[25];
    char sexo;
    char condicion[3];
} Gato;

typedef struct {
    int status; 
    char contenido[2000];
} Respuesta;

#endif //GLOBAL_H
