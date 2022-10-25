all: servidor

servidor: servidor.o 
	gcc -o servidor servidor.o lista.o

servidor.o: servidor.c
	gcc -c servidor.c lista.h lista.c

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o servidor

