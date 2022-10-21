all: main

main: main.o lista.o
	gcc -o main main.o lista.o ../monitoring.o 

main.o: main.c lista.h ../monitoring.h
	gcc -c main.c

lista: lista.o
	gcc -o lista lista.o ../monitoring.o

lista.o: lista.c
	gcc -c lista.c lista.h ../monitoring.h

clean:
	rm -f *.o
