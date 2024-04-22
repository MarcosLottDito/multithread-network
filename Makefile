all: dirs
	gcc -Wall -c common.c -o object/common.o -lm
	gcc -Wall client.c object/common.o -o bin/client -lm
	gcc -Wall -lpthread server.c object/common.o -o bin/server -lm

dirs:
	mkdir -p bin object

clean:
	@rm -f object/common.o bin/client bin/server