all: cliente

cliente: cliente.o socket.o
	gcc -o cliente cliente.o socket.o

cliente.o: cliente.c socket.h
	gcc -c cliente.c

socket.o:
	gcc -c socket.c
	
clean:
	rm	-f *.o

cleanall:
	rm	-f *.o cliente