all: mg_MEServer mg_MEClient

mg_MEServer: mg_MEServer.o prog1.o LinkedList.o
	gcc mg_MEServer.o prog1.o LinkedList.o -o mg_MEServer

mg_MEServer.o: mg_MEServer.c
	gcc -c mg_MEServer.c

mg_MEClient: mg_MEClient.o prog1.o LinkedList.o
	gcc mg_MEClient.o prog1.o LinkedList.o -o mg_MEClient

mg_MEClient.o: mg_MEClient.c
	gcc -c mg_MEClient.c

prog1.o: prog1.c
	gcc -c prog1.c

LinkedList.o: LinkedList.c
	gcc -c LinkedList.c

clean:
	rm -f mg_MEServer mg_MEClient mg_MEServer.o mg_MEClient.o prog1.o LinkedList.o
