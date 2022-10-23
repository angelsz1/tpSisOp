#include "lista.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#define min(a, b) ((a)<(b) ? (a) : (b))

Nodo* crearNodo(const void* elemento, size_t tamElemento) {
    Nodo* nodoNuevo = (Nodo*)malloc(sizeof(Nodo));
    void* elemNodo = malloc(tamElemento);

    if(!nodoNuevo || !elemNodo) {
        free(nodoNuevo);
        free(elemNodo);
        return 0;
    }

    memcpy(elemNodo, elemento, tamElemento);

    nodoNuevo->elemento = elemNodo;
    nodoNuevo->tamElemento = tamElemento;
    nodoNuevo->siguiente = NULL;

    return nodoNuevo;
}

void destruirNodo(Nodo* nodoAEliminar, void* elemento, size_t tamElemento) {
    memcpy(elemento, nodoAEliminar->elemento, min(tamElemento, nodoAEliminar->tamElemento));

    free(nodoAEliminar->elemento);
    free(nodoAEliminar);
}

void crearLista(Lista* lista) {
    *lista = NULL;
}

int insertarEnListaOrdenada(Lista* lista, const void* elemento, size_t tamElemento, FuncComparar comparar) {
    while(*lista && (comparar(elemento, (*lista)->elemento) > 0)) {
        lista = &(*lista)->siguiente;
    }
    if(*lista && (comparar(elemento, (*lista)->elemento) == 0)) {
        return 2; ///DUPLICADO
    }
    Nodo* nodoNuevo = (Nodo*)malloc(sizeof(Nodo));
    void* bloqueElem = malloc(tamElemento);
    if(!nodoNuevo || !bloqueElem) {
        free(nodoNuevo);
        free(bloqueElem);
        return 0; ///SIN ESPACIO
    }
    memcpy(bloqueElem, elemento, tamElemento);
    nodoNuevo->elemento = bloqueElem;
    nodoNuevo->tamElemento = tamElemento;
    nodoNuevo->siguiente = *lista;
    *lista = nodoNuevo;

    return 1; ///TODO OK;
}

int eliminarDeLista(Lista* lista, void* elemento, size_t tamElemento, FuncComparar comparar) {
    while (*lista && comparar(elemento, (*lista)->elemento) > 0) {
        lista = &(*lista)->siguiente;
    }
    if(!*lista || comparar(elemento, (*lista)->elemento) < 0) {
        return 0;
    }

    Nodo* nodoAEliminar = *lista;
    *lista = nodoAEliminar->siguiente;

    destruirNodo(nodoAEliminar, elemento, tamElemento);

    return 1;
}

int buscarEnLista(const Lista* lista, void* elemento, size_t tamElemento, FuncComparar comparar) {
    Nodo* actual = *lista;
    while (actual && comparar(elemento, actual->elemento) > 0) {
        actual = actual->siguiente;
    }
    if(!actual || comparar(elemento, actual->elemento) < 0) {
        return 0;
    }

    memcpy(elemento, actual->elemento, tamElemento);
    return 1;
}

void mostrarLista(Lista* lista, FuncMostrar mostrar) {
    Nodo* actual = *lista;
    while(actual) {
        mostrar(actual->elemento);
        actual = actual->siguiente;
    }
}

void cantElementosEnLista(Lista* lista, unsigned* cantElementos) {
    Nodo* actual = *lista;
    while(actual) {
        (*cantElementos)++;
        actual = actual->siguiente;
    }
}

void vaciarLista(Lista* lista) {
    while(*lista) {
        Nodo* nodoAEliminar = *lista;
        *lista = nodoAEliminar->siguiente;
        free(nodoAEliminar->elemento);
        free(nodoAEliminar);
    }
}
