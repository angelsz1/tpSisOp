#include "cat.h"

void cat_fill_from_str(const char *str, Cat *p_cat)
{
    sscanf(str, "%s %s %c %s", p_cat->name, p_cat->race, &p_cat->sex, p_cat->sexCondition);
}

void cat_fill_from_query(const Query *p_query, Cat *p_cat)
{
    strcpy(p_cat->name, p_query->name);
    strcpy(p_cat->race, p_query->race);
    p_cat->sex = p_query->sex;
    strcpy(p_cat->sexCondition, p_query->sexCondition);
}

int cat_find(const Cat *p_cat, FILE *arch, char *sm)
{
    if (!aux_check_name_exist(p_cat->name, arch))
    {
        return 0;
    }

    aux_show_line_from_file(p_cat->name, arch, sm);

    return 1;
}

int cat_delete(const Cat *p_cat, FILE *arch)
{
    char line[SIZE_QUERY];
    fseek(arch, 0L, SEEK_SET);
    Cat cat_aux;

    FILE *arch_temp = fopen("tempCat.txt", "wt");

    if (!arch_temp)
    {
        puts("Error al crear archivo temporario");
        return 0;
    }

    while (fgets(line, SIZE_QUERY, arch))
    {
        cat_fill_from_str(line, &cat_aux);

        if (strcmp(cat_aux.name, p_cat->name))
        {
            fprintf(arch_temp, "%s %s %c %s\n", cat_aux.name, cat_aux.race, cat_aux.sex, cat_aux.sexCondition);
        }
    }

    rename("tempCat.txt", "../gatos.txt");
    fclose(arch_temp);
}

// Aca hay referencia circular, pero sino esto tendria que ir a un archivo nuevo ("aux.c")

char aux_check_name_exist(const char *str, FILE *arch)
{
    char line[SIZE_QUERY];
    fseek(arch, 0, SEEK_SET);
    char find = 0;
    Cat cat;

    fgets(line, SIZE_QUERY, arch);
    while (!feof(arch) && !find)
    {
        cat_fill_from_str(line, &cat);
        if (strcmp(str, cat.name) == 0)
        {
            find = 1;
        }

        fgets(line, SIZE_QUERY, arch);
    }

    return find;
}

void aux_show_line_from_file(const char *str, FILE *arch, char *sm)
{
    char line[SIZE_QUERY];
    char res[SIZE_QUERY];
    fseek(arch, 0, SEEK_SET);
    char find = 0;
    Cat cat;

    while (fgets(line, SIZE_QUERY, arch))
    {
        cat_fill_from_str(line, &cat);

        if (strcmp(str, cat.name) == 0)
        {
            sprintf(sm, COLOR_GREEN "|%17s|%17s|%5s|%10s|\n" COLOR_RESET,
                    "Nombre", "Raza", "Sexo", "Condicion");
            sprintf(res, "|%17s|%17s|%5c|%10s|\n",
                    cat.name, cat.race, cat.sex, cat.sexCondition);
            strcat(sm, res);
            find = 1;
        }
    }
}

void aux_show_file(FILE *arch, char *sm)
{
    char line[SIZE_QUERY];
    char res[SIZE_QUERY];
    Cat cat;

    fseek(arch, 0L, SEEK_SET);
    
    sprintf(sm, COLOR_GREEN "|%17s|%17s|%5s|%10s|\n" COLOR_RESET,
            "Nombre", "Raza", "Sexo", "Condicion");   
    
    while (fgets(line, SIZE_QUERY, arch))
    {
        cat_fill_from_str(line, &cat);
        sprintf(res, "|%17s|%17s|%5c|%10s|\n",
            cat.name, cat.race, cat.sex, cat.sexCondition);
        strcat(sm, res);
    }
}
