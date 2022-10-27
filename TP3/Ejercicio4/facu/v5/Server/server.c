#include "server.h"

int server_active = 1;

int sm_id;

sem_t *mutex_memory;
sem_t *are_query;
sem_t *are_response;

void signal_handler()
{
    server_active = 0;
    sem_post(are_query);
}

void server_create()
{
    int pid;

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

    server_wait_query(shared_memory_create(&sm_id));
}

void server_wait_query(char* sm)
{
    char query[SIZE_QUERY];

    // Create the Semaphores;
    mutex_memory = sem_open("mutex", O_CREAT, 0666, 1);
    are_query = sem_open("/are_query", O_CREAT, 0666, 0);
    are_response = sem_open("/are_response", O_CREAT, 0666, 0);

    if (mutex_memory == SEM_FAILED ||
        are_query == SEM_FAILED ||
        are_response == SEM_FAILED)
    {
        puts("Hubo un error al tratar de inicializar el semaforo");
        exit(1);
    }

    while (server_active)
    {
        sem_wait(are_query);    // P
        if (!server_active) break;
        sem_wait(mutex_memory); // P
        strncpy(query, sm, sizeof(query));
        sem_post(mutex_memory); // V
        server_read_query(query, sm, mutex_memory, are_response);
    }

    // Se cierran los semaforos y la memoria compartida cuando se recibe la senial SIGUSR1
    sem_close(mutex_memory);
    sem_close(are_query);
    sem_close(are_response);

    shmdt(sm);

    // Destruimos los semaforos
    sem_unlink("mutex");
    sem_unlink("/are_query");
    sem_unlink("/are_response");
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

void server_read_query(const char *query, char *sm,
                       sem_t *mutex_memory, sem_t *are_response)
{
    Query t_query;
    query_fill_from_str(query, &t_query);

    switch (server_analyze_query(&t_query))
    {
    case 'A':
        server_up(&t_query, sm, mutex_memory, are_response);
        break;
    case 'B':
        server_down(&t_query, sm, mutex_memory, are_response);
        break;
    case 'C':
        server_query(&t_query, sm, mutex_memory, are_response);
        break;
    case 'N':
        sem_wait(mutex_memory);
        strcpy(sm, "Consulta invalida");
        sem_post(mutex_memory);
        sem_post(are_response);
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

int server_up(const Query *p_query, char *sm, sem_t *mutex_memory, sem_t *are_response)
{
    char res[SIZE_QUERY];
    strcpy(res, p_query->name);
    FILE *arch = fopen("logs.txt", "r+");
    // Verificamos que el contenido a agregar no exista
    if (!aux_check_name_exist(p_query->name, arch))
    {
        server_write(p_query, arch);
        strcat(res, " Ingresado Correctamente");
        sem_wait(mutex_memory);
        strcpy(sm, res);
        sem_post(mutex_memory);
        sem_post(are_response);
    }
    else
    {
        strcat(res, " Duplicado");
        sem_wait(mutex_memory);
        strcpy(sm, res);
        sem_post(mutex_memory);
        sem_post(are_response);
    }
    fclose(arch);
}

int server_down(const Query *p_query, char *sm, sem_t *mutex_memory, sem_t *are_response)
{
    Cat cat;
    FILE *arch = fopen("logs.txt", "r+");
    char res[SIZE_QUERY];
    strcpy(res, p_query->name);
    if (!aux_check_name_exist(p_query->name, arch))
    {
        strcat(res, " No Existe");
        sem_wait(mutex_memory);
        strcpy(sm, res);
        sem_post(are_response);
        sem_post(mutex_memory);
    }
    else
    {
        cat_fill_from_query(p_query, &cat);
        cat_delete(&cat, arch);
        strcat(res, " Eliminado");
        sem_wait(mutex_memory);
        strcpy(sm, res);
        sem_post(are_response);
        sem_post(mutex_memory);
    }
    fclose(arch);
}

int server_query(const Query *p_query, char *sm, sem_t *mutex_memory, sem_t *are_response)
{
    Cat cat;
    FILE *arch = fopen("logs.txt", "r+");
    if (strlen(p_query->name) == 0)
    {
        sem_wait(mutex_memory);
        aux_show_file(arch, sm);
        sem_post(are_response);
        sem_post(mutex_memory);
    }
    else
    {
        sem_wait(mutex_memory);
        cat_fill_from_query(p_query, &cat);
        if (!cat_find(&cat, arch, sm))
        {
            strcpy(sm, "Elemento no encontrado");
        }
        sem_post(are_response);
        sem_post(mutex_memory);
    }
    fclose(arch);
}
