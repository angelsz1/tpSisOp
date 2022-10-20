#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PATH_PRODUCTOS "Productos.txt"

typedef struct Producto
{
    int id;
    char descripcion[50];
    int precio;
    int costo;
    int stock;
} Producto;

int mostrarStockProducto();

int mostrarProductosSinStock(char*);

int mostrarCostoReposicion();

int mostrarListaProductos(char*);

int main()
{
    int fifo_1,
        fifo_2;

    // Creacion de los FIFOS
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
            mostrarProductosSinStock(textoServidor);
        }
        else if(strcasecmp(textoCliente, "LIST") == 0)
        {
            mostrarListaProductos(textoServidor);
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
                mostrarStockProducto(parametro, textoServidor);
            }
            else if(strcasecmp(comando, "REPO") == 0)
            {
                mostrarCostoReposicion(parametro, textoServidor);
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

int mostrarStockProducto(int id, char* textoServidor)
{
    FILE *file = fopen(PATH_PRODUCTOS, "r");
    if (file == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char line[100];
    Producto producto;

    rewind(file);
    fgets(line, 100, file);

    while (fscanf(file, "%d;%[^;];%d;%d;%d;\n", &producto.id, producto.descripcion, &producto.precio, &producto.costo,
                  &producto.stock) != EOF)
    {
        if (producto.id == id)
        {
            sprintf(line, "%s %du\n", producto.descripcion, producto.stock);
            strcat(textoServidor, line);
            //printf("%s %du\n", producto.descripcion, producto.stock);
            break;
        }
    }

    fclose(file);
}

int mostrarProductosSinStock(char* textoServidor)
{
    FILE *file = fopen(PATH_PRODUCTOS, "r");
    if (file == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char line[100];
    Producto producto;

    rewind(file);
    fgets(line, 100, file);

    while (fscanf(file, "%d;%[^;];%d;%d;%d;\n", &producto.id, producto.descripcion, &producto.precio, &producto.costo,
                  &producto.stock) != EOF)
    {
        if (producto.stock == 0)
        {
            sprintf(line, "%d %s $%d\n", producto.id, producto.descripcion, producto.costo);
            strcat(textoServidor, line);
            //printf("%d %s $%d\n", producto.id, producto.descripcion, producto.costo);
        }
    }

    fclose(file);

}

int mostrarCostoReposicion(int cantidadAReponer, char* textoServidor)
{
    FILE *file = fopen(PATH_PRODUCTOS, "r");
    if (file == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char line[100];
    Producto producto;

    rewind(file);
    fgets(line, 100, file);

    int totalCosto = 0;
    while (fscanf(file, "%d;%[^;];%d;%d;%d;\n", &producto.id, producto.descripcion, &producto.precio, &producto.costo,
                  &producto.stock) != EOF)
    {
        if (producto.stock == 0)
        {
            totalCosto += producto.costo * cantidadAReponer;
        }
    }
    // print the total cost of the products to reponer
    sprintf(textoServidor, "$%d\n", totalCosto);



    fclose(file);

}

int mostrarListaProductos(char* textoServidor)
{
    FILE *file = fopen(PATH_PRODUCTOS, "r");
    if (file == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char line[100];
    Producto producto;

    rewind(file);
    fgets(line, 100, file);

    // LIST
    while (fscanf(file, "%d;%[^;];%d;%d;%d;\n", &producto.id, producto.descripcion, &producto.precio, &producto.costo,
                  &producto.stock) != EOF)
    {
        sprintf(line, "%d %s $%d\n", producto.id, producto.descripcion, producto.costo);
        strcat(textoServidor, line);
        //printf("%d %s $%d\n", producto.id, producto.descripcion, producto.precio);
    }

    fclose(file);
}
