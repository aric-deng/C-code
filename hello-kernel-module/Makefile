KERNEL_DIR := /home/dhs/code/gk8602_v1.1.1_sdk/linux/kernel/3.4.43
CUR_DIR := $(shell pwd)

obj-m:=hello.o


default:
	make -C $(KERNEL_DIR) M=`pwd` modules

clean:
	-rm *.o *.ko *.mod.c *.order *.symvers
