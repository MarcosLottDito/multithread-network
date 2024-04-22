all: dirs
	gcc -Wall -c common.c -o object/common.o
	gcc -Wall client.c object/common.o -o bin/client
	gcc -Wall -lpthread server.c object/common.o -o bin/server

dirs:
	mkdir -p bin object

clean:
	@rm -f object/common.o bin/client bin/server