ifneq ($(KERNELRELEASE),)
obj-m := kernel_monitor.o
else
KDIR := /home/galaxy/Desktop/EmbeddedLinux/Linux-yocto-Excersises/linux/code/linux
PWD := $(shell pwd)
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
endif
