
CROSS := arm-linux-
CC := $(CROSS)gcc
LD := $(CROSS)ld
STRIP := $(CROSS)strip
CUR_DIR := $(shell pwd)

TARGET := audioPlayer
SRC := audioPlayer.c

DEP_INC_DIR =$(addprefix  -I, $(shell find $(CUR_DIR)/inc -type d))
DEP_LIB_DIR =$(addprefix  -L, $(shell find $(CUR_DIR)/lib -type d))

#CFLAGS := -Wall $(DEP_INC_DIR) -g -o
CFLAGS := -Wall $(DEP_INC_DIR) -o
LDFLAGS:= -lpthread -ladi $(DEP_LIB_DIR)

default: 
	$(CC) $(CFLAGS) $(TARGET) $(SRC) $(LDFLAGS)
	
clean:
	-rm $(TARGET)