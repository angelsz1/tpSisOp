#ifndef CONST_H_INCLUDED
#define CONST_H_INCLUDED

#define SEM_MUTEX_MEMORY "./mutex_memory"
#define SEM_ARE_QUERY "./are_query"
#define SEM_ARE_RESPONSE "./are_response"
#define SEM_CAN_QUERY "./can_query"
#define SEM_CAN_SEND "./can_send"

#define SIZE_QUERY 512

#define QUERY_UP "ALTA"
#define QUERY_DOWN "BAJA"
#define QUERY_QUERY "CONSULTA"

typedef struct query_t
{
    char type[50];
    char name[50];
    char race[50];
    char sex;
    char sexCondition[2];
} Query;



#endif // CONST_H_INCLUDED