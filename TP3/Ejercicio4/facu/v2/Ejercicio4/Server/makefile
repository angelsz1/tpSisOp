all: main

main: main.o 
	gcc -o main main.o 

main.o: main.c
	gcc -c main.c

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o main

# gcc -pthread -o servidor servidor.c