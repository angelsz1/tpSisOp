LIBS=	-pthread

all: lista backToDir ej2

ej2: Ejercicio2.o monitoring.o
	gcc -o ej2 Ejercicio2.o monitoring.o Lista/lista.o $(LIBS)

Ejercicio2.o: Ejercicio2.c monitoring.h 
	gcc -c Ejercicio2.c $(LIBS)

monitoring.o: monitoring.c monitoring.h Lista/lista.h Lista/lista.c
	gcc -c monitoring.c $(LIBS)

lista: 
	cd Lista/ && make


backToDir:
	cd ..

clean:
	rm -f *.o && rm -f ej2 && rm -f log.txt
	cd Lista/ && make clean
