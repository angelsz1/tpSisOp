// #include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct query_t
{
    char type[50];
    char name[50];
    char race[50];
    char sex;
    char sexCondition[2];
} Query;

Query fillQuery(const char* str, Query* p_query) {
    char type[50];
    int i = 0;

    while(*str != ' ') {
        type[i] = *str;
        i++;
        str++;
    }

    strcpy(p_query->type, type);
}

int main() {

    // server_create();
    char str[50] = {"ALTA SnowBall siames M CA"};
    Query query;
    // printf("GetQuery: %c\n", getQuery(str));
    fillQuery(str, &query);
    printf("QUERY: %s\n", query.type);
    return 0;
}