all: build
all: run

build:
	gcc -Wall -o main main.c process.c queue.c scheduler.c 

run:
	./main

