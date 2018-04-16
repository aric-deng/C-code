#CROSS := arm-linux-
CC := $(CROSS)g++
LD := $(CROSS)ld
STRIP := $(CROSS)strip
CUR_DIR := $(shell pwd)

SRC := MKVRepair.cpp
TARGET := MKVRepair

#DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
#DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))
# DEP_INC_DIR += -I$(KERNEL_DIR)

CFLAGS := -Wall -g -o

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC) $(DEP_INC_DIR)
	
clean:
	-rm $(TARGET)
