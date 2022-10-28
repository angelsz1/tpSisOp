// #APL N. 3
// #EJERCICIO 3
// #INTEGRANTES:
//       #Carballo, Facundo Nicolas (DNI: 42774931)
//       #Garcia Burgio, Matias Nicolas (DNI: 42649117)
//       #Mottura, Agostina Micaela (DNI: 41898101)
//       #Povoli Olivera, Victor (DNI: 43103780)
//       #Szust, Ángel Elías (DNI: 43098495)

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
