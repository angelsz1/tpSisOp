#ifndef lista_h
#define lista_h


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../monitoring.h"


typedef enum {false, true} boolean;
typedef struct sNodo{
    void *dato;
    unsigned tam;
    struct sNodo *sig;
}tNodo;

typedef tNodo* List;

void crearLista(List *l);
void add(List *l, void *d, unsigned t);
void *getFromList(List *l, unsigned i);
void *removeFromList(List *l, unsigned i);
void *removeFirstFromList(List *l);
boolean isEmpty(List *l);
boolean isFull(List *l);
void empty(List *l);
void destroy(List *l);
void *getByContent(List *l, void *d, unsigned t, boolean (*cmp)(void*, void*));
boolean cmp(void *a, void *b);
void *removeByContent(List *l, void *d, unsigned t, boolean (*cmp)(void*, void*));
void toString(List *l, void (*print)(void*));
void print(void *a);

#endif /* lista_h */