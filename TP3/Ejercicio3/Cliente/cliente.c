// #APL N. 3
// #EJERCICIO 3
// #INTEGRANTES:
//       #Carballo, Facundo Nicolas (DNI: 42774931)
//       #Garcia Burgio, Matias Nicolas (DNI: 42649117)
//       #Mottura, Agostina Micaela (DNI: 41898101)
//       #Povoli Olivera, Victor (DNI: 43103780)
//       #Szust, Ángel Elías (DNI: 43098495)

#include "funciones.h"

#define MENSAJE_AYUDA "Sintaxis: ./servidor\n"                                                              \
                      "LISTA COMANDOS\n"                                                                    \
                      "1) STOCK [producto_id]\n"                                                            \
                      "Muestra DESCRIPCIÓN y STOCK para un producto dado.\n"                               \
                      "2) SIN_STOCK\n"                                                                      \
                      "Muestra ID, DESCRIPCIÓN y COSTO de los productos con STOCK cero.\n"                 \
                      "3) REPO [cantidad]\n"                                                                \
                      "Muestra el costo total de reponer una cantidad dada para cada producto sin stock.\n" \
                      "4) LIST\n"                                                                           \
                      "Muestra ID, DESCRIPCIÓN y PRECIO de todos los productos existentes.\n"              \
                      "5) QUIT\n"                                                                           \
                      "Finaliza la ejecución.\n"

int main(int argc, char *argv[])
{
    // Ignoro CTRL + C
    signal(SIGINT, SIG_IGN);

    // Validar parametros
    if (argc > 2)
    {
        printf("No se requieren parametros para ejecutar el cliente. Para obtener ayuda agregar el parametro -h o --help\n");
        return 1;
    }
    if (argc == 2 && (strcasecmp(argv[1], "-h") == 0 || strcasecmp(argv[1], "--help") == 0))
    {
        printf(MENSAJE_AYUDA);
        return 2;
    }
    else if (argc == 2)
    {
        printf("No se requieren parametros para ejecutar el cliente. Para obtener ayuda agregar el parametro -h o --help\n");
        return -1;
    }

    // Creacion de los FIFOS
    int fifo_1,
        fifo_2;

    char *FIFO1 = "/tmp/FIFO1";
    char *FIFO2 = "/tmp/FIFO2";

    mkfifo(FIFO1, 0666);
    mkfifo(FIFO2, 0666);

    // Cadena auxiliares para para leer y escribir
    char textoCliente[200],
        textoServidor[1000000];

    while (1)
    {
        pedirComando(textoCliente);

        if (validarSintaxis(textoCliente))
        {

            // Abro FIFO1 para escribir la cadena textoCliente y luego lo cierro
            fifo_1 = open(FIFO1, O_WRONLY);
            write(fifo_1, textoCliente, strlen(textoCliente) + 1);
            close(fifo_1);

            // Abro FIFO2 para leer con la cadena textoServidor y luego lo cierro
            fifo_2 = open(FIFO2, O_RDONLY);
            read(fifo_2, textoServidor, sizeof(textoServidor));
            close(fifo_2);

            // Si se escribio QUIT se debe cerrar el cliente tambien
            if (textoServidor[0] == 'Q')
            {
                remove("/tmp/FIFO2");

                return 0;
            }

            // Muestro la respuesta del servidor
            printf("\n%s\n", textoServidor);
        }
    }
    return 0;
}
