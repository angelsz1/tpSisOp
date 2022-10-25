#include "query.h"

void query_fill_from_str(const char* str, Query* p_query)
{

    strcpy(p_query->name, "");
    strcpy(p_query->race, "");
    p_query->sex = '\0';
    strcpy(p_query->sexCondition, "");
    strcpy(p_query->type, "");

    const char* act = strrchr(str, ' ');
    
    if (!act) {
        // Sabemos que es una consulta simple
        sscanf(str, "%s", p_query->type);
        return;
    }

    if (strcmp(act, " CA") == 0 || strcmp(act, " SC") == 0) {
        // Sabemos que es un ALTA
        sscanf(str, "%s %s %s %c %s", p_query->type, p_query->name, p_query->race, &p_query->sex, p_query->sexCondition);
        return;
    }
    
    // Sabemos que tiene si o si dos parametros el type y el name
    sscanf(str, "%s %s", p_query->type, p_query->name);

}

void query_print(Query* p_query) 
{
    printf("Type: %s\n", p_query->type);
    printf("Name: %s\n", p_query->name);
    printf("Race: %s\n", p_query->race);
    printf("Sex: %c\n", p_query->sex);
    printf("Sex Condition: %s\n", p_query->sexCondition);
}
