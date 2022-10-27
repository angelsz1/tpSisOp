# all: socket main

# main: main.o Socket/socket.o
# 	gcc -o main main.o Socket/socket.o

# main.o: main.c Socket/socket.h
# 	gcc -c main.c

# socket:
# 	cd Socket/ && make
# 	cd ..

# clean:
# 	rm	-f *.o

# cleanall:
# 	rm	-f *.o main


all: main

main: main.o socket.o server.o cat.o shared_memory.o query.o sem.o client.o
	@gcc -o main main.o socket.o server.o cat.o shared_memory.o query.o sem.o client.o -pthread

main.o: main.c socket.h server.h
	gcc -c main.c -pthread

socket.o: socket.c
	gcc -c socket.c -pthread
	
server.o: server.c server.h
	gcc -c server.c -pthread

cat.o: cat.c query.h
	gcc -c cat.c -pthread

shared_memory.o: shared_memory.c query.h
	gcc -c shared_memory.c -pthread

query.o: query.c
	gcc -c query.c -pthread

sem.o: sem.c
	gcc -c sem.c -pthread

client.o: client.c
	gcc -c client.c -pthread

clean:
	rm	-f *.o

cleanall:
	rm	-f *.o main
