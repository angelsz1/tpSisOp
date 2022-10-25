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

main: main.o socket.o
	gcc -o main main.o socket.o

main.o: main.c socket.h
	gcc -c main.c

socket.o:
	gcc -c socket.c
	
clean:
	rm	-f *.o

cleanall:
	rm	-f *.o main
