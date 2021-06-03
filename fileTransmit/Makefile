#******************************************************************************
# * Makefile
# *
# * Copyright (C) 2011-2013 ZheJiang Dahua Technology CO.,LTD.
# *
# * Author : lbbxsxlz <lbbxsxlz@gmail.com>
# * Version: V1.0.0   Created@2021年04月16日,上午09:36:48
# *
# * Makefile for compiling fileTransmit
# 
# * Version: V2.0
# * Support Multi-files compiling
# ******************************************************************************
plat := x86_64
PWD:=$(shell pwd)

CC := gcc
CFLAGS := -Wall -g -O2
TARGETS := fileTransmit

ifeq ($(plat), hi3516a)
CC = arm-hisiv300-linux-gcc
CFLAGS += -DHI3516A
else ifeq ($(plat), s3lm)
CC = arm-linux-gnueabihf-5.2.1-gcc
CFLAGS += -DS3LM
else ifeq ($(plat), hi3559av100)
CC = aarch64-himix100v630-linux-gcc
CFLAGS += -DHI3559AV100
else ifeq ($(plat), hi3559av100_sdk031)
CC = aarch64-himix100v630r3-linux-gcc
CFLAGS += -DHI3559AV100
else ifeq ($(plat), hi3516dv300)
CC = arm-himix200v002-linux-gcc
CFLAGS += -DHI3516DV300
else ifeq ($(plat), hi3519av100)
CC = arm-himix200-linux-gcc
CFLAGS += -DHI3519AV100
else ifeq ($(plat), cv22)
CC = aarch64-linux-gnu-8.2.1-gcc
CFLAGS += -DAMBCV22
else ifeq ($(plat), ssc335de)
CC = arm-linux-gnueabihf-gcc4.9.4-uclibc1.0.31-ssc335-v1-gcc
CFLAGS += -DSSC335DE
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
