ARCH ?= x86_64
CROSS_COMPILE ?=
KDIR ?= /DATA/binbin/workspace/linux-6.5-rc5
CC = $(CROSS_COMPILE)gcc
PWD = $(shell pwd)

ifeq ($(ARCH), riscv)
KDIR = /DATA/binbin/workspace/linux
endif

obj-m := test.o

all:default
default:
        $(MAKE) -C $(KDIR) M=$(PWD) modules
        mkdir -p $(ARCH)
        cp test.ko $(ARCH)
clean:
        rm -rf *.o *.ko *.mod.c .*.cmd .tmp_versions
