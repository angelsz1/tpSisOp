all: cliente

cliente: cliente.o funciones.o
	gcc -o cliente cliente.o funciones.o

cliente.o: cliente.c funciones.h
	gcc -c cliente.c

funciones.o: funciones.c
	gcc -c funciones.c

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o cliente
