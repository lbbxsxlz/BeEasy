CC ?= gcc
CFLAGS := -Wall -g -O2 -I include
server := server
client := client

all: $(server) $(client)

$(server): server.o lib/example.o lib/util.o
	$(CC) $(CFLAGS) -o $@ server.o lib/smbios.o lib/util.o

$(client): sdbus-client.o lib/util.o
	$(CC) $(CFLAGS) -o $@ client.o lib/util.o

clean:
	rm -f *.o $(client) $(server)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
