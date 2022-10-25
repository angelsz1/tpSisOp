#ifndef QUERY_H_INCLUDED
#define QUERY_H_INCLUDED

#include <stdio.h>
#include <string.h>

#define SIZE_QUERY 512

#define QUERY_UP 'A'
#define QUERY_DOWN 'B'
#define QUERY_QUERY 'C'


typedef struct query_t
{
    char type[50];
    char name[50];
    char race[50];
    char sex;
    char sexCondition[2];
} Query;

void query_fill_from_str(const char* str, Query* p_query);

void query_print(Query* p_query);

#endif // QUERY_H_INCLUDED