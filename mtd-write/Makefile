CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
#CUR_DIR := $(shell pwd)
KERNEL_DIR := /home/dhs/krnl-v134/3.4.43

SRC := mtd-write.c
TARGET := mtd-write

DEP_INC_DIR = -I$(KERNEL_DIR)

CFLAGS := -Wall -g -o

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC) $(DEP_INC_DIR)
	
clean:
	-rm $(TARGET)
