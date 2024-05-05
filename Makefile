
CFLAGS = -g -Wall               

all: webserv my-histogram

my-histogram.o: my-histogram.c
	gcc -c my-histogram.c

my-histogram: my-histogram.o
	gcc -o my-histogram my-histogram.o

webserv: cache.o webserv.c             # 'webserv' depends on 'cache.o' and 'webserv.c'
	gcc $(CFLAGS) -o webserv cache.o webserv.c

cache.o: cache.c cache.h              # 'cache.o' depends on 'cache.c' and 'cache.h'
	gcc $(CFLAGS) -c cache.c



clean:
	rm -rf *.o webserv my-histogram output.txt histogram.jpeg
