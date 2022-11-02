LIBS=	-pthread

all: servidor

servidor: servidor.o lista.o
	gcc -o servidor servidor.o lista.o $(LIBS)

servidor.o: servidor.c lista.h
	gcc -c servidor.c $(LIBS)

lista.o:
	gcc -c lista.c

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o servidor lista.h.gch

