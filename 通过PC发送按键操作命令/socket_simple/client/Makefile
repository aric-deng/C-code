CROSS_COMPILE = arm-none-linux-gnueabi-

#CC = $(CROSS_COMPILE)gcc
#AR = $(CROSS_COMPILE)ar
CC = gcc
AR = ar
#CFLAGS = -O -Wall
CFLAGS = -g -Wall
INCLUDE = -I../inc
LIB 	= -L../lib
DEFINE = -DDUMMY

OUTPUT = client 

OBJS := $(patsubst %.c, %.o, $(wildcard *.c))

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	#$(AR) cr $(OUTPUT) $^
	$(CC) -o $(OUTPUT) $^ $(LIB)

%.o:%.c
	$(CC) $(DEFINE) $(INCLUDE) $(CFLAGS) -c $^

clean:
	@rm -f *.o *~ core $(OUTPUT) 
