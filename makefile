ifeq ($(DEBUG), true)
	CC = gcc -g
else
	CC = gcc
endif

all: main.o terminal.o handle_input.o
	$(CC) -o output main.o terminal.o handle_input.o

terminal.o: terminal.c terminal.h
	$(CC) -c terminal.c

handle_input.o: handle_input.c handle_input.h
	$(CC) -c handle_input.c

run:
	./output

clean:
	rm *.o