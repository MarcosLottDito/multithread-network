all:
	gcc -Wall -c common.c
	gcc -Wall client.c common.o -o client
	gcc -Wall -lpthread server.c common.o -o server

clean:
	@rm -f common.o client server