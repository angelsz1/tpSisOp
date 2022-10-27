all: mainCliente

mainCliente: mainCliente.o 
	gcc -o mainCliente mainCliente.o -pthread

mainCliente.o: mainCliente.c
	gcc -c mainCliente.c -pthread

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o main