all: servidor

servidor: servidor.o socket.o server.o cat.o query.o sem.o client.o
	@gcc -o servidor servidor.o socket.o server.o cat.o query.o sem.o client.o -pthread

servidor.o: servidor.c socket.h server.h
	gcc -c servidor.c -pthread

socket.o: socket.c
	gcc -c socket.c -pthread
	
server.o: server.c server.h
	gcc -c server.c -pthread

cat.o: cat.c query.h
	gcc -c cat.c -pthread

query.o: query.c
	gcc -c query.c -pthread

sem.o: sem.c
	gcc -c sem.c -pthread

client.o: client.c
	gcc -c client.c -pthread

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o servidor
