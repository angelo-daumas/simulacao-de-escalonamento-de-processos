CC = gcc
CFLAGS  = -g -Wall -D EXAMPLE

default: build
default: run
default: clean

run: build
	./simulador

build:  process.o queue.o scheduler.o creator.o devices.o
	@$(CC) $(CFLAGS) -o simulador main.c process.o queue.o scheduler.o creator.o devices.o

process.o:  process.c process.h 
	@$(CC) $(CFLAGS) -c process.c

queue.o:  queue.c queue.h 
	@$(CC) $(CFLAGS) -c queue.c

scheduler.o:  process.h scheduler.c scheduler.h 
	@$(CC) $(CFLAGS) -c scheduler.c

creator.o:  process.h creator.c creator.h
	@$(CC) $(CFLAGS) -c creator.c

devices.o:  devices.c devices.h
	@$(CC) $(CFLAGS) -c devices.c

clean: 
	@$(RM) count *.o *~ simulador
