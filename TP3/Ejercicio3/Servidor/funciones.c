#include "funciones.h"

int mostrarStockProducto(int id, char *textoServidor, char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char line[1000];
    Producto producto;

    rewind(file);
    fgets(line, 1000, file);

    while (fscanf(file, "%d;%[^;];%d;%d;%d;\n", &producto.id, producto.descripcion, &producto.precio, &producto.costo,
                  &producto.stock) != EOF)
    {
        if (producto.id == id)
        {
            sprintf(line, "%s %du\n", producto.descripcion, producto.stock);
            strcat(textoServidor, line);
            // printf("%s %du\n", producto.descripcion, producto.stock);
            break;
        }
    }

    fclose(file);
}

int mostrarProductosSinStock(char *textoServidor, char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char line[1000];
    Producto producto;

    rewind(file);
    fgets(line, 1000, file);

    while (fscanf(file, "%d;%[^;];%d;%d;%d;\n", &producto.id, producto.descripcion, &producto.precio, &producto.costo,
                  &producto.stock) != EOF)
    {
        if (producto.stock == 0)
        {
            sprintf(line, "%d %s $%d\n", producto.id, producto.descripcion, producto.costo);
            strcat(textoServidor, line);
            // printf("%d %s $%d\n", producto.id, producto.descripcion, producto.costo);
        }
    }

    fclose(file);
}

int mostrarCostoReposicion(int cantidadAReponer, char *textoServidor, char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char line[1000];
    Producto producto;

    rewind(file);
    fgets(line, 1000, file);

    int totalCosto = 0;
    while (fscanf(file, "%d;%[^;];%d;%d;%d;\n", &producto.id, producto.descripcion, &producto.precio, &producto.costo,
                  &producto.stock) != EOF)
    {
        if (producto.stock == 0)
        {
            totalCosto += producto.costo * cantidadAReponer;
        }
    }

    sprintf(textoServidor, "$%d\n", totalCosto);

    fclose(file);
}

int mostrarListaProductos(char *textoServidor, char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    char line[1000];
    Producto producto;

    rewind(file);
    fgets(line, 1000, file);

    while (fscanf(file, "%d;%[^;];%d;%d;%d;\n", &producto.id, producto.descripcion, &producto.precio, &producto.costo,
                  &producto.stock) != EOF)
    {
        sprintf(line, "%d %s $%d\n", producto.id, producto.descripcion, producto.costo);
        strcat(textoServidor, line);
        // printf("%d %s $%d\n", producto.id, producto.descripcion, producto.precio);
    }

    fclose(file);
}