ifeq ($(DEBUG), true)
	CC = gcc -g -lpthread
else
	CC = gcc -lpthread
endif

all: main.o terminal.o handle_input.o display.o
	$(CC) -o output main.o terminal.o handle_input.o display.o

terminal.o: terminal.c terminal.h
	$(CC) -c terminal.c

handle_input.o: handle_input.c handle_input.h
	$(CC) -c handle_input.c

display.o: display.c display.h
	$(CC) -c display.c

run:
	./output

clean:
	rm *.o