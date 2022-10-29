#include "lista.h"
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>
#include "../global.h"

#define min(a, b) ((a)<(b) ? (a) : (b))

int compararGato(const void* gato1, const void* gato2);

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

    Gato gato;
    FILE* archivoGatos = fopen("gatos", "rb");
    
    if (!archivoGatos) {
        puts("Error abriendo el archivo");
    }

    fread(&gato, sizeof(Gato), 1, archivoGatos);
    while (!feof(archivoGatos))
    {
        insertarEnListaOrdenada(lista, &gato, sizeof(Gato));
        fread(&gato, sizeof(Gato), 1, archivoGatos);
    }

    fclose(archivoGatos);
}

int insertarEnListaOrdenada(Lista* lista, const void* elemento, size_t tamElemento) {
    while(*lista && (compararGato(elemento, (*lista)->elemento) > 0)) {
        lista = &(*lista)->siguiente;
    }
    if(*lista && (compararGato(elemento, (*lista)->elemento) == 0)) {
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

int eliminarDeLista(Lista* lista, void* elemento, size_t tamElemento) {
    while (*lista && compararGato(elemento, (*lista)->elemento) > 0) {
        lista = &(*lista)->siguiente;
    }
    if(!*lista || compararGato(elemento, (*lista)->elemento) < 0) {
        return 0;
    }

    Nodo* nodoAEliminar = *lista;
    *lista = nodoAEliminar->siguiente;

    destruirNodo(nodoAEliminar, elemento, tamElemento);

    return 1;
}

int buscarEnLista(const Lista* lista, void* elemento, size_t tamElemento) {
    Nodo* actual = *lista;
    while (actual && compararGato(elemento, actual->elemento) > 0) {
        actual = actual->siguiente;
    }
    if(!actual || compararGato(elemento, actual->elemento) < 0) {
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
    FILE* archivoGatos = fopen("gatos", "wb");
    
    while(*lista) {
        Nodo* nodoAEliminar = *lista;
        
        if (!archivoGatos) {
            puts("Error abriendo el archivo");
        }

        fwrite(nodoAEliminar->elemento, sizeof(Gato), 1, archivoGatos);
        *lista = nodoAEliminar->siguiente;
        free(nodoAEliminar->elemento);
        free(nodoAEliminar);
    }

    fclose(archivoGatos);
}

void listarTodos(Lista* lista, void* texto) {
    Gato gato;
    char aux[70];

    if(!(*lista)) {
        strcpy(texto, "No hay gatos en el refugio");
        return;
    }

    sprintf (texto, GREEN"|%17s|%17s|%5s|%10s|\n"RESET, 
    "Nombre", "Raza", "Sexo", "Condicion");
    while(*lista) {
        memcpy(&gato, (*lista)->elemento, sizeof(Gato));
        sprintf (aux, "|%17s|%17s|%5c|%10s|\n", 
        gato.nombre, gato.raza, gato.sexo, gato.condicion);
        lista = &(*lista)->siguiente;
        strcat(texto, aux);
    }
}

int compararGato(const void* gato1, const void* gato2) {
    Gato g1 = *(Gato*)gato1;
    Gato g2 = *(Gato*)gato2;
    return strcmp(g1.nombre, g2.nombre);
}