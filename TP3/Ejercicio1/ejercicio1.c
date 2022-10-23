#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>

void informeDeProceso(int generacion, char tipo[]);

int main(int argc, char *argv[])
{

  printf("PID   |GENERACION |PPID  |PARENTEZCO  \n");

  informeDeProceso(1, "Padre");

  sem_t* finRama = sem_open( "/finRamaDeArbol", O_CREAT, 0666, 0);
  sem_t* continuar = sem_open( "/continuarProceso", O_CREAT, 0666, 0);

  if( !fork() ) // hijo1
  {
    informeDeProceso(2, "Hijo");
    
    if( !fork() ) //nieto1
    {
      informeDeProceso(3, "Nieto");

      if( !fork() ) //bis1
      {
        informeDeProceso(4, "Bisnieto");

        sem_post(finRama);
        sem_wait(continuar);
      }
      else if( !fork() ) //bis2
      {
        informeDeProceso(4, "Bisnieto");

        sem_post(finRama);
        sem_wait(continuar);
      }
      else //sigue nieto1
      {
        sem_wait(continuar);
      }
    }
    else if( !fork() )//nieto2
    {
      informeDeProceso(3, "Nieto");

      if( !fork() ) //bis3
      {
        informeDeProceso(4, "Bisnieto");

        sem_post(finRama);
        sem_wait(continuar);
      }
      else if( !fork() ) //bis4
      {
        informeDeProceso(4, "Bisnieto");

        sem_post(finRama);
        sem_wait(continuar);
      }
      else //sigue nieto2
      {
        sem_wait(continuar);
      }
    }
    else //sigue hijo1
    {
      sem_wait(continuar);
    }
  }
  else if ( !fork() ) //hijo2
  {
    informeDeProceso(2, "Hijo");
    
    if( !fork() ) //nieto3 -> ZOMBIE
    {
      informeDeProceso(3, "Zombie");

      if( !fork() )//bisnieto5 -> DEMONIO
      {
        informeDeProceso(4, "Demonio");

        sem_post(finRama);
        sem_wait(continuar);
      }
      //no hay else con sem_wait(continuar), es el zombie
    }
    else if( !fork() )//nieto4
    {
      informeDeProceso(3, "Nieto");

      if( !fork() )//bisnieto6 -> ZOMBIE
      {
        informeDeProceso(4, "Zombie");

        sem_post(finRama);
        //no hace sem_wait(continuar) por ser zombie
      }
      else if ( !fork() )//bisnieto7
      {
        int i;

        informeDeProceso(4, "Bisnieto");
        
        for(i=0; i<6; i++)
          sem_wait(finRama);

        printf("Ingrese una tecla para finalizar:");
        char fin;
        scanf("%c", &fin);

        for(i=0; i<11; i++)
          sem_post(continuar);
      }
      else //sigue nieto 4
      {
        sem_wait(continuar);
      }
    }
    else //sigue hijo2
    {
      sem_wait(continuar);
    }
  }
  else //padre
  {
    sem_wait(continuar);
  }

  return 0;
}

void informeDeProceso(int generacion, char tipo[])
{
  printf( "%-6d|%-11d|%-6d|%-13s\n", getpid(), generacion, getppid(), tipo);
}