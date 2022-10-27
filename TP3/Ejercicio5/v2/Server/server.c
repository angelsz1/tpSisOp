#include "server.h"

int server_active = 1;

void signal_handler(int sig)
{

    server_active = 0;
}

void server_create()
{
    int pid, id;

    // Ignoramos la senial SIGINT (CTRL + C)
    signal(SIGINT, SIG_IGN);

    // Hacemos que la senial SIGUSR1 termine el servidor
    signal(SIGUSR1, &signal_handler);

    // Creamos un proceso "hijo"
    pid = fork();

    if (pid > 0)
    {
        exit(0); // Finalizamos el proceso padre. Logrando que el proceso que creamos se ejecute en segundo plano
    }
    else if (pid < 0)
    {
        exit(1);
    }

    //  server_wait_query(shared_memory_create(&id));
}

void server_wait_query(Client *p_client)
{
    char query[SIZE_QUERY];

    // Create the Semaphores;
    sem_t *mutex_memory = sem_open("mutex", O_CREAT, 0666, 1);
    sem_t *are_query = sem_open("/are_query", O_CREAT, 0666, 0);       //
    sem_t *are_response = sem_open("/are_response", O_CREAT, 0666, 0); //

    if (mutex_memory == SEM_FAILED ||
        are_query == SEM_FAILED ||
        are_response == SEM_FAILED)
    {
        puts("Hubo un error al tratar de inicializar el semaforo");
        exit(1);
    }

    // server_read_query(p_client, mutex_memory, are_response);


}

char server_analyze_query(const Query *p_query)
{
    if (
        strcmp(p_query->type, "ALTA") == 0 &&
        strlen(p_query->name) != 0 &&
        strlen(p_query->race) != 0 &&
        (p_query->sex == 'M' || p_query->sex == 'H') &&
        (strcmp(p_query->sexCondition, "CA") == 0 || strcmp(p_query->sexCondition, "SC") == 0))
    {
        return 'A';
    }

    if (
        strcmp(p_query->type, "BAJA") == 0 &&
        strlen(p_query->name) > 0 &&
        strlen(p_query->race) == 0 &&
        p_query->sex == '\0' &&
        strlen(p_query->sexCondition) == 0)
    {
        return 'B';
    }

    if (
        strcmp(p_query->type, "CONSULTA") == 0 &&
        strlen(p_query->race) == 0 &&
        p_query->sex == '\0' &&
        strlen(p_query->sexCondition) == 0)
    {
        return 'C';
    }

    return 'N';
}

void server_read_query(Client *p_client)
{
    Query t_query;
    query_fill_from_str(p_client->buffer_client, &t_query);

    switch (server_analyze_query(&t_query))
    {
    case 'A':
        server_up(&t_query, p_client);
        break;
    case 'B':
        server_down(&t_query, p_client);
        break;
    case 'C':
        server_query(&t_query, p_client);
        break;
    case 'N':
        write(p_client->socket_number, "Consulta Invalida...", sizeof("Consulta Invalida"));
        break;
    }
}

char server_check_name_exist(const Query *p_query, FILE *arch)
{
    char line[SIZE_QUERY];
    fseek(arch, 0, SEEK_SET);
    char find = 0;
    Query s_query;

    fgets(line, SIZE_QUERY, arch);
    while (!feof(arch) && !find)
    {
        query_fill_from_str(line, &s_query);

        if (strcmp(p_query->name, s_query.name) == 0)
        {
            find = 1;
        }

        fgets(line, SIZE_QUERY, arch);
    }

    return find;
}

void server_write(const Query *p_query, FILE *arch)
{
    fprintf(arch, "%s %s %c %s\n", p_query->name, p_query->race, p_query->sex, p_query->sexCondition);
}

int server_up(const Query *p_query, Client *p_client)
{
    char res[SIZE_QUERY];
    strcpy(res, p_query->name);
    FILE *arch = fopen("logs.txt", "r+");

    if (!aux_check_name_exist(p_query->name, arch))
    {
        server_write(p_query, arch);
        strcat(res, " Ingresado Correctamente");
    }
    else
    {
        strcat(res, " Duplicado");
    }

    write(p_client->socket_number, res, sizeof(res));
    
    fclose(arch);
}

int server_down(const Query *p_query, Client *p_client)
{
    Cat cat;
    FILE *arch = fopen("logs.txt", "r+");
    char res[SIZE_QUERY];
    strcpy(res, p_query->name);
    if (!aux_check_name_exist(p_query->name, arch))
    {
        strcat(res, " No Existe");
    }
    else
    {
        cat_fill_from_query(p_query, &cat);
        cat_delete(&cat, arch);
        strcat(res, " Eliminado");
    }

    write(p_client->socket_number, res, sizeof(res));

    fclose(arch);
}

int server_query(const Query *p_query, Client *p_client)
{
    Cat cat;
    FILE *arch = fopen("logs.txt", "r+");

    if (strlen(p_query->name) == 0)
    {
        aux_show_file(arch, p_client->buffer_client);
    }
    else
    {
        cat_fill_from_query(p_query, &cat);
        if (!cat_find(&cat, arch, p_client->buffer_client))
        {
            strcpy(p_client->buffer_client, "Elemento no encontrado");
        }
    }

    write(p_client->socket_number, p_client->buffer_client, sizeof(p_client->buffer_client));

    fclose(arch);
}
