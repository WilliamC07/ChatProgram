ifeq ($(DEBUG), true)
	CC = gcc -g -lpthread -pthread
else
	CC = gcc -lpthread -pthread
endif

all: main.o terminal.o handle_input.o display.o chat.o storage.o server.o
	$(CC) -o output main.o terminal.o handle_input.o display.o chat.o storage.o server.o

main.o: main.c terminal.h display.h handle_input.h
	$(CC) -c main.c

terminal.o: terminal.c terminal.h
	$(CC) -c terminal.c

handle_input.o: handle_input.c handle_input.h display.h
	$(CC) -c handle_input.c

display.o: display.c display.h chat.h
	$(CC) -c display.c

chat.o: chat.c chat.h handle_input.h
	$(CC) -c chat.c

storage.o: storage.c
	$(CC) -c storage.c

server.o: server.c
	$(CC) -c server.c

run:
	./output

clean:
	rm *.o