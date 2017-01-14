CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
#CUR_DIR := $(shell pwd)
KERNEL_DIR := /home/dhs/code/gk8602_v1.1.1_sdk/linux/kernel/3.4.43
DEP_INC_DIR = -I$(KERNEL_DIR)

# target source
SRC  := $(wildcard *.c) 
OBJ  := $(SRC:%.c=%.o)
CFLAGS := -Wall -g

TARGET := ir-demo
.PHONY : clean all

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(DEP_INC_DIR)
#	$(STRIP) $(TARGET)

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJ)

