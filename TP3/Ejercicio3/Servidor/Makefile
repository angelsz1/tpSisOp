all: servidor

servidor: servidor.o funciones.o
	gcc -o servidor servidor.o funciones.o

servidor.o: servidor.c funciones.h
	gcc -c servidor.c

funciones.o: funciones.c
	gcc -c funciones.c

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o servidor
