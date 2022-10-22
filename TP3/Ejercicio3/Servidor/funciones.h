#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <signal.h>

#include <pwd.h>
#include <errno.h>

typedef struct Producto
{
    int id;
    char descripcion[50];
    int precio;
    int costo;
    int stock;
} Producto;

int mostrarStockProducto(int id, char *textoServidor, char *path);

int mostrarProductosSinStock(char *textoServidor, char *path);

int mostrarCostoReposicion(int cantidadAReponer, char *textoServidor, char *path);

int mostrarListaProductos(char *textoServidor, char *path);

#endif // FUNCIONES_H_INCLUDED
