all: webserv my-histogram

webserv.o: webserv.c
	gcc -c webserv.c 

my-histogram.o: my-histogram.c
	gcc -c my-histogram.c

webserv: webserv.o
	gcc -o webserv webserv.o

my-histogram: my-histogram.o
	gcc -o my-histogram my-histogram.o

clean:
	rm -f *.o webserv my-histogram output.txt histogram.jpeg