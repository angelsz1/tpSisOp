LIBS=	-pthread

all: servidor

servidor: servidor.o 
	gcc -o servidor servidor.o lista.o $(LIBS)

servidor.o: servidor.c
	gcc -c servidor.c lista.h lista.c $(LIBS)

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o servidor lista.h.gch

