CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
CUR_DIR := $(shell pwd)

#TARGET := audioPlayer
SRC := mem.c
TARGET := mem

DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))

#use -g3 to debug macro	
CFLAGS := -Wall -g3 -o

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC)
	
clean:
	-rm $(TARGET)
