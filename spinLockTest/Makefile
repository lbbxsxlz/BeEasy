plat:=?
ifeq ($(plat), hi3516dv300)
cross = arm-himix200v002-linux-
KDIR = /home/hisi/linux-4.9.37-hi3516dv300
else ifeq ($(plat), hi3559av100)
cross =  aarch64-himix100v630-linux-
KDIR = /home/hisi/Linux-4.9.37_HI3559AV100ES/src
endif

CC = $(cross)gcc

ifneq ($(KERNELRELEASE),)
obj-m := spinLockTest.o 
else
	PWD := $(shell pwd)
endif

all:default
default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	mkdir -p $(PWD)/$(plat)
	cp spinLockTest.ko $(PWD)/$(plat)
clean:
	rm -rf *.o *.ko *.mod.c .*.cmd .tmp_versions
	rm $(PWD)/$(plat)/*.ko
