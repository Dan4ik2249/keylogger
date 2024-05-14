MODULE_NAME :=keylogger
USERSPACE := getlog
obj-m :=$(MODULE_NAME).o
 
KERNELDIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
 
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
	gcc $(USERSPACE).c -o $(USERSPACE)
 
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
	rm $(USERSPACE)
 
