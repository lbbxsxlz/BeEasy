#******************************************************************************
# * Makefile
# *
# * Author : lbbxsxlz <lbbxsxlz@gmail.com>
# * Version: V2.0
# * Support Multi-files compiling
# ******************************************************************************
plat := x86_64
PWD:=$(shell pwd)

CC := gcc
CFLAGS := -Wall -g -O2
TARGETS := fileTransmit

ifeq ($(plat), hi3559av100)
CC = aarch64-himix100v630-linux-gcc
else ifeq ($(plat), hi3516dv300)
CC = arm-himix200v002-linux-gcc
else ifeq ($(plat), hi3519av100)
CC = arm-himix200-linux-gcc
endif

#INC := $(wildcard ./include/*.h)
#SRC := $(wildcard ./src/*.c)
INC := $(wildcard ./*.h)
SRC := $(wildcard ./*.c)

OBJ := $(patsubst %.c,%.o,$(SRC))
DEP := $(patsubst %.c,%.d,$(SRC))

all: $(OBJ) $(TARGETS)

clean:
	rm -f $(OBJ) $(DEP) $(TARGETS)
	rm -rf *~

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(TARGETS):$(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	-mkdir -p $(PWD)/bin/$(plat)
	-mv $(TARGETS) $(PWD)/bin/$(plat)
