LIBS=	-pthread

all: cliente

cliente: cliente.o 
	gcc -o cliente cliente.o $(LIBS)

cliente.o: cliente.c
	gcc -c cliente.c $(LIBS)

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o cliente
