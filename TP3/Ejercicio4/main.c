#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

int main()
{
    pid_t process_id = 0;

    process_id = fork();
    if(process_id < 0) {
        printf("fork failed!\n");
        exit(1);
    }
    if (process_id > 0) {
        printf("process_id of child process %d \n", process_id);
        exit(0);
    }

    FILE* archivo = fopen("archivo.txt", "rb");
    while (1)
    {
        sleep(1);
        fprintf(archivo, "Logging info...\n");
    }
    fclose(archivo);
    return (0);
}
