all: socket

socket: socket.o
	gcc -o socket socket.o

socket.o: socket.c
	gcc -c socket.c

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o socket