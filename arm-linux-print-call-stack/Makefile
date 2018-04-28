CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
CUR_DIR := $(shell pwd)

SRC := seg.c
TARGET := seg

DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))

CFLAGS := -Wall -g -rdynamic -funwind-tables -fasynchronous-unwind-tables -mapcs-frame -o

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC)
	
clean:
	-rm $(TARGET)
