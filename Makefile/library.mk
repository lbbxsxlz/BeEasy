CC ?= gcc
CFLAGS := -Wall -g -O2
staticlib := libexample.a
dynamiclib := libexample.so

all: $(staticlib) $(dynamiclib)

$(staticlib): example.o util.o
        $(AR) rcs $@ $?

$(dynamiclib): example.c util.c
        $(CC) -fPIC -shared $? -o $@

clean:
        rm -f *.o $(staticlib) $(dynamiclib)

%.o: %.c %.h
        $(CC) $(CFLAGS) -c -o $@ $<
