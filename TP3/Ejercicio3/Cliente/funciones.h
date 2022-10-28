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

#include <ctype.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <signal.h>

void pedirComando(char *textoCliente);

int validarSintaxis(char *textoCliente);

int esNumero(char *cadena);

#endif // FUNCIONES_H_INCLUDED
