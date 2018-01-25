CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
CUR_DIR := $(shell pwd)

SRC := $(wildcard *.c)
TARGET := $(patsubst %.c, %, $(SRC))

DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))
#DEP_INC_DIR += -I$(KERNEL_TOP)/include

CFLAGS := -Wall -g -o

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC)
	
clean:
	-rm $(TARGET)
