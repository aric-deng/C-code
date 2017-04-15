 
#=============================================#
#CROSS ?= arm-gk-linux-gnueabi-
export CC = $(CROSS)gcc
export AR = $(CROSS)ar
export AS = $(CROSS)as
export LD = $(CROSS)ld
export CROSS_T = $(CROSS)

export CP = cp
export RM = rm
export INSTALL = cp -rfv

# ============= DIR ===========# 
ROOTDIR = $(PWD)
EXP_TARGET_DIR ?= $(ROOTDIR)/install/lib/uvc/
EXP_INC_DIR    ?= $(ROOTDIR)/install/inc/uvc/

#KERNEL_DIR = $(KERNEL_TOP)
#KERNEL_INCLUDE  := -I$(KERNEL_DIR)/include -I$(KERNEL_DIR)/arch/$(ARCH)/include

#DEP_INC_DIR =$(addprefix  -I../, $(shell find $(ROOTDIR)/inc -type d -path ${ROOTDIR}/inc/uvc  -prune -o -print))
DEP_INC_DIR =$(addprefix  -I../, $(shell find $(ROOTDIR)/inc -type d))
DEP_INC_DIR +=$(addprefix  -I../, $(shell find $(ROOTDIR)/demo  -type d))
#DEP_INC_DIR += -I $(ROOTDIR)/demo
# ============= demo compile option ===========#
DEMO_TARGET := uvc_host_demo
DEMO_SRC    := ./demo
DEMO_CFLAGS := -Wall -I./ -I$(EXP_INC_DIR) $(KERNEL_INCLUDE) 
DEMO_LDFLAGS:= -pthread -luvc_host 
#DEMO_LDFLAGS:= -lpthread  -lz -lfreetype -lavencode -lz -lparam -lstreamBuff  -Wl,-rpath=$(EXP_TARGET_DIR)  -L./ -L$(EXP_TARGET_DIR) -L$(KERNEL_INCLUDE)
# ---------------------------------------------#
DEMO_MAKE_PARAM := -C $(DEMO_SRC)

# ============= lib compile option ===========#
#HISI_INC_PATH:=$(PWD)/

LIB_TARGET  := libuvc_host.a
EXP_LIB_INC := $(PWD)/inc/uvc_host.h
LIB_SRC     := ./src
LIB_CFLAGS  := -Wall -fpic -g -I./ -I../inc  $(KERNEL_INCLUDE)  $(DEP_INC_DIR)


LIB_LDFLAGS := -lpthread -L./
# ---------------------------------------------#
LIB_MAKE_PARAM := -C $(LIB_SRC) \
		TARGET=$(LIB_TARGET)\
		EXPORT_INC_FILE=$(EXP_LIB_INC)\
		EXPORT_TARGET_DIR=$(EXP_TARGET_DIR)\
		EXPORT_INC_DIR=$(EXP_INC_DIR)\
		CFLAGS="$(LIB_CFLAGS)"\
		LDFLAGS="$(LIB_LDFLAGS)"


.PHONY: default clean

#default:
all:
	make $(LIB_MAKE_PARAM)
	make $(DEMO_MAKE_PARAM)

clean:
	make clean $(LIB_MAKE_PARAM)
	make clean $(DEMO_MAKE_PARAM)




