#include "lista.h"

int main(){
    List l;
    crearLista(&l);
    for(int i = 0; i < 10; i++){
        add(&l, &i, sizeof(int));
    }

    for(int i = 0; i < 10; i++){
        printf("%d\n", *(int*)removeFirstFromList(&l));
        printf("%s\n", isEmpty(&l) ? "true" : "false");
    }

    return 0;
}