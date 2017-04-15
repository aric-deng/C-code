#CROSS ?= arm-linux-
CC = $(CROSS)gcc
AR = $(CROSS)ar
AS = $(CROSS)as
LD = $(CROSS)ld

TARGET := uvc_host_demo
SRC := uvc_host_demo.c

INSTALL_PATH = ../
DEP_INC_DIR =-I$(INSTALL_PATH)inc
DEP_LIB_DIR =-L$(INSTALL_PATH)src

CFLAGS := -Wall -g $(DEP_INC_DIR) -o
LDFLAGS:= -pthread -luvc_host $(DEP_LIB_DIR)

uvc_host_demo:
	$(CC) $(CFLAGS) $(TARGET) $(SRC) $(LDFLAGS)
	
clean:
	-rm $(TARGET)
