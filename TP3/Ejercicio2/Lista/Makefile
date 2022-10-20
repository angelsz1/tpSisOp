all: main

main: main.o lista.o
	gcc -o main main.o lista.o 

main.o: main.c lista.h 
	gcc -c main.c

lista: lista.o
	gcc -o lista lista.o 

lista.o: lista.c
	gcc -c lista.c lista.h 

clean:
	rm -f *.o && rm -f *.gch && rm -f main 
