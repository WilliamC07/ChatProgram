ifeq ($(DEBUG), true)
	CC = gcc -g
else
	CC = gcc
endif

all: main.o terminal.o
	$(CC) -o output main.o terminal.o

terminal.o: terminal.c terminal.h
	$(CC) -c terminal.c

run:
	./output

clean:
	rm *.o