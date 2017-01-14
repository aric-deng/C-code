CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
CUR_DIR := $(shell pwd)

SRC := uart-demo.c
TARGET := uart-demo

DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))

CFLAGS := -Wall -g -o
LDFLAGS:= -lpthread

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC) $(LDFLAGS)
	
clean:
	-rm $(TARGET)
