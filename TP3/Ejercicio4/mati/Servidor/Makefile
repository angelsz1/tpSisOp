all: servidor

servidor: servidor.o 
	gcc -o servidor servidor.o 

servidor.o: servidor.c
	gcc -c servidor.c

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o servidor

