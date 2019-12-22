ifeq ($(DEBUG), true)
	CC = gcc -g -lpthread
else
	CC = gcc -lpthread
endif

all: main.o terminal.o handle_input.o display.o
	$(CC) -o output main.o terminal.o handle_input.o display.o

main.o: main.c terminal.h display.h handle_input.h
	$(CC) -c main.c

terminal.o: terminal.c terminal.h
	$(CC) -c terminal.c

handle_input.o: handle_input.c handle_input.h display.h
	$(CC) -c handle_input.c

display.o: display.c display.h
	$(CC) -c display.c

run:
	./output

clean:
	rm *.o