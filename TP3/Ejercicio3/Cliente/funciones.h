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
