all: webserv

webserv.o: webserv.c
	gcc -c webserv.c 

webserv: webserv.o
	gcc -o webserv webserv.o

clean:
	rm -f *.o webserv  