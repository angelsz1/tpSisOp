#ifndef CAT_H_INCLUDED
#define CAT_H_INCLUDED


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "query.h"
#include "cliente.h"


#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RESET "\x1b[0m"

typedef struct cat_t
{
    char name[50];
    char race[50];
    char sex;
    char sexCondition[2];
} Cat;

void cat_fill_from_str(const char* str, Cat* p_cat);
void cat_fill_from_query(const Query* p_query, Cat* p_cat);
int cat_find(const Cat* p_cat, FILE* arch, char*);
int cat_delete(const Cat* p_cat, FILE* arch);

void hola();
char aux_check_name_exist(const char* str, FILE *arch);
void aux_show_line_from_file(const char* str, FILE* arch, char*);
void aux_show_file(FILE* arch, char*);



#endif