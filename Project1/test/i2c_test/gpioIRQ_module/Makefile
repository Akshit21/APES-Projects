MODULE_NAME = gpioIRQ
obj-m += $(MODULE_NAME).o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

test:
	sudo dmesg -C
	sudo insmod $(MODULE_NAME).ko
	sudo rmmod $(MODULE_NAME).ko
	dmesg
