all: cliente

cliente: cliente.o 
	gcc -o cliente cliente.o 

cliente.o: cliente.c
	gcc -c cliente.c

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o cliente

