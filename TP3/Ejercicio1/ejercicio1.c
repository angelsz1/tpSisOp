#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

void informeDeProceso(int generacion, char tipo[]);
void mostrarAyuda(void);

int main(int argc, char *argv[])
{
  if ( argc > 2 || ( argc == 2 && strcmp( argv[1], "-h" ) != 0 && strcmp( argv[1], "--help" ) != 0 ) )
  {
    printf("Parametros incorrectos.\nPara ver ayuda:   ./ejercicio1 -h || ./ejercicio1 --help\n ");
    return -1;
  }
  else if ( argc == 2 )
  { 
    mostrarAyuda();
    return 1;
  }

  printf("PID%4s|GENERACION%1s|PPID%3s|PARENTEZCO/TIPO\n", "", "", "");

  informeDeProceso(1, "Padre");

  sem_t* finRama = sem_open( "/finRamaDeArbol", O_CREAT, 0666, 0);
  sem_t* continuar = sem_open( "/continuarProceso", O_CREAT, 0666, 0);
  sem_t* continuarUltimo = sem_open( "/continuarUltimoProceso", O_CREAT, 0666, 0);

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
        sem_post(continuarUltimo);
      }
      else if( !fork() ) //bis2
      {
        informeDeProceso(4, "Bisnieto");

        sem_post(finRama);
        sem_wait(continuar);
        sem_post(continuarUltimo);
      }
      else //sigue nieto1
      {
        sem_wait(continuar);
        sem_post(continuarUltimo);
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
        sem_post(continuarUltimo);
      }
      else if( !fork() ) //bis4
      {
        informeDeProceso(4, "Bisnieto");

        sem_post(finRama);
        sem_wait(continuar);
        sem_post(continuarUltimo);
      }
      else //sigue nieto2
      {
        sem_wait(continuar);
        sem_post(continuarUltimo);
      }
    }
    else //sigue hijo1
    {
      sem_wait(continuar);
      sem_post(continuarUltimo);
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
        sem_post(continuarUltimo);
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

        for(i=0; i<10; i++)
          sem_post(continuar);
      }
      else //sigue nieto 4
      {
        sem_wait(continuar);
        sem_post(continuarUltimo);
      }
    }
    else //sigue hijo2
    {
      sem_wait(continuar);
      sem_post(continuarUltimo);
    }
  }
  else //padre
  {
    int i;
    for(i=0; i<10; i++)
      sem_wait(continuarUltimo);

    sem_destroy(finRama);
    sem_destroy(continuar);
    sem_destroy(continuarUltimo);
  }

  return 0;
}

void mostrarAyuda(void)
{
  printf("La ejecucion de este programa genera:\n2 procesos hijos\n3 procesos nietos\n5 procesos bisnietos\n1 proceso demonio\n--------------------------------------------------------------\nDe cada proceso se ve: PID, GENERACION, PPID, TIPO DE PROCESO\n--------------------------------------------------------------\nPuede verificar la jerarquia de procesos desde otra terminal\ncon los comandos: ps -fea    |    pstree -p\n--------------------------------------------------------------\nPara finalizar la ejecucion ingresa una tecla seguida de ENTER\n\n");
}

void informeDeProceso(int generacion, char tipo[])
{
  printf( "%-7d|%-11d|%-7d|%-13s\n", getpid(), generacion, getppid(), tipo);
}