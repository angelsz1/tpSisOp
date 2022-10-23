all: ejercicio1

ejercicio1: ejercicio1.o
	gcc -pthread -o ejercicio1 ejercicio1.o

ejercicio1.o: ejercicio1.c
	gcc -c ejercicio1.c

cleanall:
	rm -f *.o ejercicio1