CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
CUR_DIR := $(shell pwd)
KERNEL_DIR := /home/dhs/krnl-v136/3.4.43

SRC := chk_gsensor_irq.c
TARGET := chk_gsensor_irq

#DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
#DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))
DEP_INC_DIR += -I$(KERNEL_DIR)

CFLAGS := -Wall -g -o

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC) $(DEP_INC_DIR)
	
clean:
	-rm $(TARGET)
