CROSS := 
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
CUR_DIR := $(shell pwd)

#TARGET := audioPlayer
SRC := dup2.c 
TARGET := dup2 

DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))

CFLAGS := -Wall -g -o

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC)
	
clean:
	-rm $(TARGET)
