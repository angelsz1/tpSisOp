// #APL N. 3
// #EJERCICIO 3
// #INTEGRANTES:
//       #Carballo, Facundo Nicolas (DNI: 42774931)
//       #Garcia Burgio, Matias Nicolas (DNI: 42649117)
//       #Mottura, Agostina Micaela (DNI: 41898101)
//       #Povoli Olivera, Victor (DNI: 43103780)
//       #Szust, Ángel Elías (DNI: 43098495)

#include "funciones.h"

#define MENSAJE_AYUDA "Sintaxis: ./servidor [RUTA_ARCHIVO_PRODUCTOS]\n" \
                      "Formato archivo: .txt\n"                         \
                      "Separador de campos: ';'\n"                      \
                      "Los campos deben tener encabezado, ejemplo:\n"   \
                      "ID;DESCRIPCION;PRECIO;COSTO;STOCK\n"             \
                      "1;Harina Blancaflor;70;40;0;\n"

int main(int argc, char *argv[])
{
    // Ignoro CTRL + C
    signal(SIGINT, SIG_IGN);

    // Validar parametros
    if (argc > 2)
    {
        printf("Cantidad de argumentos invalidos, debe ingresar solamente la ruta del archivo de productos. Para obtener ayuda agregar el parametro -h o --help\n");
        return 1;
    }
    if (argc == 2 && (strcasecmp(argv[1], "-h") == 0 || strcasecmp(argv[1], "--help") == 0))
    {
        // TODO AYUDA
        printf(MENSAJE_AYUDA);
        return 2;
    }
    else if (argc == 1)
    {
        printf("Se requiere como parametro la ruta del archivo de productos. Para obtener ayuda agregar el parametro -h o --help\n");
        return -1;
    }

    char *path = argv[1];

    // Valido ruta del archivo
    struct stat sts;
    if (stat(path, &sts) == -1)
    {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(1);
    }
    else if (S_ISDIR(sts.st_mode))
    {
        fprintf(stderr, "%s No es un archivo valido\n", path);
        exit(1);
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
        // Abro FIFO1 para leer solamente mediante la cadena textoCliente y luego lo cierro
        fifo_1 = open(FIFO1, O_RDONLY);
        read(fifo_1, textoCliente, 80);
        close(fifo_1);

        // Ejecuto el comando correspondiente
        if (strcasecmp(textoCliente, "SIN_STOCK") == 0)
        {
            mostrarProductosSinStock(textoServidor, argv[1]);
        }
        else if (strcasecmp(textoCliente, "LIST") == 0)
        {
            mostrarListaProductos(textoServidor, argv[1]);
        }
        else if (strcasecmp(textoCliente, "QUIT") == 0)
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

            if (strcasecmp(comando, "STOCK") == 0)
            {
                mostrarStockProducto(parametro, textoServidor, path);
            }
            else if (strcasecmp(comando, "REPO") == 0)
            {
                mostrarCostoReposicion(parametro, textoServidor, path);
            }
        }

        // Abro FIFO2 para escribir solamente mediante la cadena textoServidor y luego lo cierro
        fifo_2 = open(FIFO2, O_WRONLY);
        write(fifo_2, textoServidor, strlen(textoServidor) + 1);
        close(fifo_2);

        // "Vacio" la cadena textoServidor para luego escribir la salida de otro comando diferente o el mismo
        textoServidor[0] = '\0';
    }
    return 0;
}
