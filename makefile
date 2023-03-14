CC = gcc
HEADERS = -pthread

all: server client

server: Server.c
	$(CC) $(HEADERS) -o server Server.c

client: Client.c
	$(CC) $(HEADERS) -o client Client.c

clean:
	rm -f server client

fresh:
	clean all
