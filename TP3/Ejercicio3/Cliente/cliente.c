#include "funciones.h"

int main()
{
    signal(SIGINT, nada);

    // Creacion de los FIFOS
    int fifo_1,
        fifo_2;

    char* FIFO1 = "/tmp/FIFO1";
    char* FIFO2 = "/tmp/FIFO2";

    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);

    // Cadena auxiliares para para leer y escribir
    char    textoCliente[100],
            textoServidor[2000];

    while (1)
    {
        pedirComando(textoCliente);

        if(validarSintaxis(textoCliente))
        {



            // Abro FIFO1 para escribir la cadena textoCliente y luego lo cierro
            fifo_1 = open(FIFO1, O_WRONLY);
            write(fifo_1, textoCliente, strlen(textoCliente)+1);
            close(fifo_1);

            // Abro FIFO2 para leer con la cadena textoServidor y luego lo cierro
            fifo_2 = open(FIFO2, O_RDONLY);
            read(fifo_2, textoServidor, sizeof(textoServidor));
            close(fifo_2);

            // Si se escribio QUIT se debe cerrar el cliente tambien
            if(textoServidor[0] == 'Q')
            {
                return 0;
            }

            // Muestro la respuesta del servidor
            printf("%s", textoServidor);
        }

    }
    return 0;
}

