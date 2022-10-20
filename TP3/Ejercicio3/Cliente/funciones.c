#include "funciones.h"


void pedirComando(char* textoCliente)
{
    printf("Ingrese comando: ");
    fgets(textoCliente, 100, stdin);

    char* pos = strchr(textoCliente, '\n');
    *pos = '\0';
}

int validarSintaxis(char* textoCliente)
{
    if(strcasecmp(textoCliente, "SIN_STOCK") == 0 ||
       strcasecmp(textoCliente, "LIST") == 0 ||
       strcasecmp(textoCliente, "QUIT") == 0)
    {
        return 1;
    }
    else
    {
        char comando[100];
        char parametro[100];

        sscanf(textoCliente, "%s %s", comando, parametro);

        if( (   strcasecmp(comando, "STOCK") == 0 ||
                strcasecmp(comando, "REPO") == 0 ) &&
                esNumero(parametro))
        {
            return 1;
        }
    }

    puts("Comando incorrecto");

    return 0;
}

int esNumero(char* cadena)
{
    if(*cadena == '\0')
        return 0;

    while(*cadena)
    {
        if(!isdigit(*cadena))
            return 0;
        cadena++;
    }

    return 1;
}

void nada()
{

}
