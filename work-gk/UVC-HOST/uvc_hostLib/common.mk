
#=============================================#
SRCPATH:=$(shell pwd | xargs basename)
SOURCES:=$(shell find . -type f -name "*.c")
HSOURCE:=$(shell find . -type f -name "*.h")

	

#=============================================#
OBJECTS :=$(addsuffix .o, $(basename $(SOURCES)))

COMPILE.c   := $(CC)   $(CFLAGS) -c
__LINK    := $(CC)   $(CFLAGS)  $(LDFLAGS)


#=============================================#
ifndef TARGET
	ERROR0 := "TARGET IS NULL!" 
	ERR_F := 1
endif

ifndef ERR_F
	TARGET_IS_LIB := $(findstring .a;,$(TARGET);)
endif

ifndef ERR_F
	ifneq ($(TARGET_IS_LIB),)
		TARGET_LIB := $(TARGET)
		ifndef EXPORT_INC_FILE
			ERROR1 := "EXPORT_INC_FILE IS NULL!"
			ERR_F := 1
		endif
	endif
	ifeq ($(TARGET_IS_LIB),)
		TARGET_ELF := $(TARGET) 
	endif
endif

ifdef ERR_F
define ERROR
	$(ERROR0)
	$(ERROR1)
endef
endif


.PHONY: default clean

ifndef ERR_F
deflaut:$(TARGET_ELF) $(TARGET_LIB)
else
deflaut:
	$(error $(ERROR))
endif

ifneq ($(TARGET_IS_LIB),)
$(TARGET_LIB):$(OBJECTS)
	@echo -e " " 
	@echo "================== LIB TYPE : $(TARGET_LIB) =================="
	$(AR) -r $@ $(OBJECTS) 
#	$(CROSS_T)strip $@	
	$(INSTALL) $(TARGET) $(EXPORT_TARGET_DIR)	
	$(INSTALL) $(EXPORT_INC_FILE) $(EXPORT_INC_DIR)	
endif

ifeq ($(TARGET_IS_LIB),)
$(TARGET_ELF):$(OBJECTS)
	@echo -e " " 
	@echo "================== EXE TYPE :  $(TARGET_ELF) =================="
	$(__LINK) -o $@  $(OBJECTS) $(LDFLAGS)
#	$(INSTALL) $(TARGET) $(EXPORT_TARGET_DIR)	
	$(CROSS_T)strip $@
#	$(RM) -f /run1/$@
	$(INSTALL) $@ /opt/goke_7101s/rootfs_glibc
endif

$(OBJECTS): %.o: %.c
	@echo "================== COMPILE : $< -o $@ ==================" 
	$(COMPILE.c) $< -o $@


clean:	
	-$(RM) $(TARGET) $(OBJECTS) -rf




