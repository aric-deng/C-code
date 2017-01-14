CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
KERNEL_DIR := /home/dhs/code/gk8602_v1.1.1_sdk/linux/kernel/3.4.43

SRC := i2c-read.c 
TARGET := i2c-read
#SRC := i2c-write.c 
#TARGET := i2c-write


#DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
#DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))
DEP_INC_DIR = -I$(KERNEL_DIR)

CFLAGS := -Wall -g -o

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC) $(DEP_INC_DIR)
	
clean:
	-rm $(TARGET)
