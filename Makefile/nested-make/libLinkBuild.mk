CC ?= gcc
CFLAGS := -Wall -g -O2 -I include
server := server
client := client
lib := lib/libexample.a

all: $(lib) $(server) $(client)

$(lib):
	$(MAKE) -C lib

$(server): server.o $(lib)
	$(CC) $(CFLAGS) -o $@ server.o -L. $(lib)

$(client): client.o $(lib)
	$(CC) $(CFLAGS) -o $@ client.o -L. $(lib)

clean:
	$(MAKE) -C lib clean
	rm -f *.o $(server) $(client)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
