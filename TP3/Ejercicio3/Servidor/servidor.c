#include "funciones.h"

int main(int argc, char *argv[])
{
    // Validar parametros
    if(argc != 2)
    {
        printf("Cantidad de argumentos invalidos, debe ingresar uno\n");
        return 1;
    }
    if(strcasecmp(argv[1], "-h") == 0 || strcasecmp(argv[1], "--help") == 0)
    {
        // TODO AYUDA
        printf("Ayuda\n");
        return 2;
    }

    signal(SIGINT, nada);

    // Creacion de los FIFOS
    int fifo_1,
        fifo_2;

    char* FIFO1 = "/tmp/FIFO1";
    char* FIFO2 = "/tmp/FIFO2";

    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);

    // Cadena auxiliares para para leer y escribir
    char    textoCliente[80],
            textoServidor[2000];

    while (1)
    {
        // Abro FIFO1 para leer solamente mediante la cadena textoCliente y luego lo cierro
        fifo_1 = open(FIFO1,O_RDONLY);
        read(fifo_1, textoCliente, 80);
        close(fifo_1);

        // Ejecuto el comando correspondiente
        if(strcasecmp(textoCliente, "SIN_STOCK") == 0)
        {
            mostrarProductosSinStock(textoServidor, argv[1]);
        }
        else if(strcasecmp(textoCliente, "LIST") == 0)
        {
            mostrarListaProductos(textoServidor, argv[1]);
        }
        else if(strcasecmp(textoCliente, "QUIT") == 0)
        {
            // Abro FIFO2 para escribir solamente mediante la cadena textoServidor y luego lo cierro
            // Entonces el cliente si detecta que lo que habia en el FIFO era una Q entonces debe cerrarse
            fifo_2 = open(FIFO2, O_WRONLY);
            write(fifo_2, "Q", 2);
            close(fifo_2);

            return 0;
        }
        else
        {
            char comando[20];
            int parametro;

            sscanf(textoCliente, "%s %d", comando, &parametro);

            if(strcasecmp(comando, "STOCK") == 0)
            {
                mostrarStockProducto(parametro, textoServidor, argv[1]);
            }
            else if(strcasecmp(comando, "REPO") == 0)
            {
                mostrarCostoReposicion(parametro, textoServidor, argv[1]);
            }
        }


        // Abro FIFO2 para escribir solamente mediante la cadena textoServidor y luego lo cierro
        fifo_2 = open(FIFO2, O_WRONLY);
        write(fifo_2, textoServidor, strlen(textoServidor)+1);
        close(fifo_2);

        // "Vacio" la cadena textoServidor para luego escribir la salida de otro comando diferente o el mismo
        textoServidor[0] = '\0';
    }
    return 0;
}

