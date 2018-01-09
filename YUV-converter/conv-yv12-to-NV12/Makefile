#CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
#CUR_DIR := $(shell pwd)
#KERNEL_DIR := /home/dhs/code/gk8602_v1.1.1_sdk/linux/kernel/3.4.43

SRC := conv-yv12-to-nv12.c
TARGET := conv-yv12-to-nv12

#DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
#DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))
#DEP_INC_DIR = -I$(KERNEL_DIR)

CFLAGS := -Wall -g -o
#LDFLAGS := -lavencode -lfreetype -lz

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC) $(DEP_INC_DIR)
	
clean:
	-rm $(TARGET)
