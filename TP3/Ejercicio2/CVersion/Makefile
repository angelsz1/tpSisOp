LIBS=	-pthread

all: ej2

ej2: Ejercicio2.o header.o
	gcc -o ej2 Ejercicio2.o header.o $(LIBS)

Ejercicio2.o: Ejercicio2.c header.h 
	gcc -c Ejercicio2.c $(LIBS)

header.o: header.c header.h
	gcc -c header.c $(LIBS)

clean:
	rm -f *.o
