#ifndef LISTA_H
#define LISTA_H
#include <stddef.h>

typedef int (*FuncComparar)(const void*, const void*);
typedef int (*FuncAccion)(const void*, void*);
typedef void (*FuncMostrar)(const void*);
typedef void (*FuncCopiar)(const void*, void*);

typedef struct sNodo {
    void* elemento;
    size_t tamElemento;
    struct sNodo* siguiente;
}Nodo;

typedef Nodo* Lista;

typedef struct {
    Nodo* inicio;
    Nodo* actual;
} Iterador;

void crearLista(Lista* lista);
int insertarEnListaOrdenada(Lista* lista, const void* elemento, size_t tamElemento);
int buscarEnLista(const Lista* lista, void* elemento, size_t tamElemento);
int eliminarDeLista(Lista* lista, void* elemento, size_t tamElemento);
void ordenarLista(Lista* lista);
void mostrarLista(Lista* lista, FuncMostrar mostrar);
void vaciarLista(Lista* lista);
void cantElementosEnLista(Lista* lista, unsigned* cantElementos);
void listarTodos(Lista* lista, void* texto);
#endif // LISTA_H
