#include "server.h"

void server_create()
{
   int pid, id;

    // Ignoramos la senial SIGINT (CTRL + C)
    signal(SIGINT, SIG_IGN);

    // Creamos un proceso "hijo"
    pid = fork();

    if (pid > 0) {
        exit(0); // Finalizamos el proceso padre. Logrando que el proceso que creamos se ejecute en segundo plano
    }else if (pid < 0) {
        return -1;
    }

     server_wait_query(shared_memory_create(&id));
}

void server_wait_query(const char* sm) 
{
    char query[SIZE_QUERY];

    // Create the Semaphores
    sem_t* mutex_memory = sem_open(SEM_MUTEX_MEMORY, O_CREAT, 0666, 1);
    sem_t* are_query = sem_open(SEM_ARE_QUERY, O_CREAT, 0666, 1);
    sem_t* are_response = sem_open(SEM_ARE_RESPONSE, O_CREAT, 0666, 1);
    sem_t* can_query = sem_open(SEM_CAN_QUERY, O_CREAT, 0666, 1);
    sem_t* can_send = sem_open(SEM_CAN_SEND, O_CREAT, 0666, 1);

    if (!mutex_memory || !are_query || !are_response || !can_query || !can_send) {
        printf("Hubo un error al tratar de inicializar el semaforo");
        exit(1);
    }

    FILE* arch = fopen("logs.txt", "r+");

    while(1) 
    {
        sem_wait(are_query); // P
        sem_wait(mutex_memory); // P
        strncpy(query, sm, sizeof(query));
        sem_post(mutex_memory); // V
        server_read_query(query, sm, mutex_memory, are_response, can_send, arch);
        sem_post(can_query); // V
    }

    fclose(arch);
    // TODO: Repasar esto, porque no se bien cuando cerrar los semaforos
    sem_close(mutex_memory);
    sem_unlink(SEM_MUTEX_MEMORY);
    sem_close(are_query);
    sem_unlink(SEM_ARE_QUERY);
}

void server_read_query(const char* query, const char* sm,
                        sem_t* mutex_memory, sem_t* are_response,
                        sem_t* can_send, FILE* arch) 
{
    char type_query = server_get_query(query);

    switch (type_query)
    {
    case 'A':
        server_up(query, arch, mutex_memory, are_response, can_send);
        break;
    case 'B':
        server_down(query, arch, mutex_memory, are_response, can_send);
        break;
    case 'C':
        server_query(query, arch, mutex_memory, are_response, can_send);
        break;
    default:
        puts("Consulta cancelada...");
        break;
    }
}

char server_get_query(const char* query) 
{
    char res = 'N';
    char* short_query = (char*) malloc(sizeof(char)*10);

    char* aux = strchr(query, ' ');

    if (aux) 
    {
        *aux = '\0';
        strcpy(short_query, query);
        *aux = ' ';
    } else
    {
        puts("La consulta tiene errores o no respeta el formato adecuado");
        free(short_query);

        return res;
    }

    if 
    (
        strcmp(short_query, QUERY_UP) != 0 ||
        strcmp(short_query, QUERY_DOWN) != 0 ||
        strcmp(short_query, QUERY_QUERY) != 0
    )
    {
        puts("La consulta tiene errores o no respeta el formato adecuado");
        free(short_query);

        return res;
    }

    res = toupper(*short_query);

    return res;
}

char* server_get_name_from_query(const char* query) 
{
    // ALTA Facundo Siames M SC
    char* from = strchr(query, ' ');
    char* name = (char*)malloc(sizeof(char)*SIZE_QUERY);
    
    if (!from) 
    {
        free(name);
        return NULL;
    }

    from++;

    while(*from != ' ' && *from != '\n' && *from != '\0')
    {
        *name = *from;
        name++;
        from++;
    }

    *name = '\0';

    return name;
}

char* server_get_name_from_file(const char* line) 
{
    // A Facundo Siames M SC
    char* name = (char*)malloc(sizeof(char)*SIZE_QUERY);

    while(*line != ' ' && *line != '\n' && *line != '\0')
    {
        *name = *line;
        name++;
        line++;
    }

    *name = '\0';

    return name;
}

char* server_get_line_to_write(const char* query, char condition)
{
    // query = ALTA Facundo Siames M SC
    char* line = (char*)malloc(sizeof(char)*SIZE_QUERY);
    *line = condition;
    line++;
    line = ' ';
    line++;

    char* from = strchr(query, ' ');
    from++;

    strcpy(line, from);

    return line;
}

char* server_check_name_exist(const char* name, FILE* arch)
{
    char line[SIZE_QUERY];
    int findName = 0;
    char* line_from_file = (char*)malloc(sizeof(char)*SIZE_QUERY);

    fseek(arch, 0, SEEK_SET);

    while(!feof(arch) && !findName)
    {
        fgets(line, SIZE_QUERY, arch);

        char* name_from_file = server_get_name_from_file(line);
        
        if ( *line == 'A' & strcmp(name, name_from_file) == 0)
        {
            findName = 1;
            strcpy(line_from_file, line);
        }
    }

    if (!findName)
    {
        free(line_from_file);
        return NULL;
    }

    return line_from_file;
}

int server_up(const char* query, FILE* arch, sem_t* mutex_memory, sem_t* are_response, sem_t* can_send)
{
    char* name = server_get_name(query);
    char* line_to_write = NULL;

    // Verificamos que el contenido a agregar no exista
    if (server_check_name_exist(name, arch))
    {
        return 0; // Si existe, devolvemos 0 (no pudimos procesarlo)
    }
        
    // Si no exite, nos posicionamos al final del archivo y escribimos los datos
        // Datos a escribir: A NOMBRE RAZA SEXO CA (el primer caracter sirve para identificar si un dato esta dado de alta o de baja)
    fseek(arch, 0, SEEK_END);
    line_to_write = server_get_line_to_write(query, 'A');
    fprintf(arch, line_to_write);
    
    // TODO: Informar al cliente que su peticion fue realizada con exito

}

int server_down(const char* query, FILE* arch, sem_t* mutex_memory, sem_t* are_response, sem_t* can_send)
{
        char* name = server_get_name(query);
        char* line_to_write = NULL;
        // Verificamos que el contenido a quitar exista
        if (!server_check_name_exist(name, arch))
        {
            return 0; // Si no existe, devolvemos 0 (no pudimos procesarlo)
        }
        
        // Si existe, nos posicionamos en el registro y lo eliminamos
        fseek(arch, -(long)SIZE_QUERY, SEEK_CUR);
        line_to_write = server_get_line_to_write(query, 'B');
        fprintf(arch, line_to_write);

        // TODO: Informar al cliente que su peticion fue realizada con exito
}

int server_query(const char* query, FILE* arch, sem_t* mutex_memory, sem_t* are_response, sem_t* can_send)
{
    char* name = server_get_name_from_query(query);
    // Verificamos si la query busca un nombre en especial
    if (!name)
    {
        // Si no, mostramos por pantalla todos los datos del archivo

        return 1;
    }
    
    // Si es asi, recorremos el archivo en busca de ese nombre
    char* line_from_file = server_check_name_exist(name, arch);
    if (!line_from_file)
    {
        return 0; // No existe el nombre buscado
    }

    // TODO: Informar al cliente que su peticion fue realizada con exito
    // mostrando por pantalla la linea obtenida
}