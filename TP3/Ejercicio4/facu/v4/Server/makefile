all: mainServer

mainServer: mainServer.o server.o cat.o query.o shared_memory.o sem.o
	@gcc -o mainServer mainServer.o server.o cat.o query.o shared_memory.o sem.o -pthread

mainServer.o: mainServer.c server.h cat.h query.h
	gcc -c mainServer.c -pthread

server.o: server.c cat.h query.h shared_memory.h sem.h
	gcc -c server.c -pthread

cat.o: cat.c query.h
	gcc -c cat.c -pthread

query.o: query.c
	gcc -c query.c -pthread

shared_memory.o: shared_memory.c query.h
	gcc -c shared_memory.c -pthread

sem.o: sem.c
	gcc -c sem.c -pthread
	
clean:
	rm	-f *.o

cleanall:
	rm	-f *.o mainServer