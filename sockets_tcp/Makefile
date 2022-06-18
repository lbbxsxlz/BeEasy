#******************************************************************************
# * Makefile
# *
# * Author : lbbxsxlz@gmail.com
# * Version: V1.0.0   Created@2022年06月17日,15:20:48
# *
# * Makefile for build Socket samples
# 
# ******************************************************************************
CC := gcc
CFLAGS := -Wall -g -O2

all: server client

server: server.c socks.h
	$(CC) server.c socks.h -o server $(CFLAGS)
client: client.c socks.h
	$(CC) client.c socks.h -o client $(CFLAGS)
clean:
	rm -f *.o server client
