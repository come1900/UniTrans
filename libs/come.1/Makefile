#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# Makefile - \library\come.1
#
# Copyright (C) 2025 ezlibs.com, All Rights Reserved.
#
# $Id: Makefile $
#
# explain
#      come.1 protocol library - JSON message codec
#
# Update:
#    2025-02-13 Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
# openwrt 2026
#PLATFORM = linux-mipsel-openwrt-linux
# cross 2026
#PLATFORM = linux-x86-64

PUB_MAKE_FILE_PREFIX =	../../../../library/Makefile.Defines
include		$(PUB_MAKE_FILE_PREFIX)/Makefile.Batch.Build

# define default platform
ifndef PLATFORM
PLATFORM = linux
endif
#PLATFORM = linux-mipsel-openwrt-linux
VERSION="1.0.0"

# CPP, CFLAGS, LIBS
include		$(PUB_MAKE_FILE_PREFIX)/Makefile.Defines.$(PLATFORM)
#for LIB_OBJS
PREFIX_BASEDIR_SRC=./src
include $(PREFIX_BASEDIR_SRC)/Makefile.SrcLists

PREFIX =	$(HOME)/libs
INS_DIR_LIB = $(PREFIX)/lib
INS_DIR_INC = $(PREFIX)/include/come1

LIB_TARGET = libcome1-$(PLATFORM).a

CFLAGS +=      -std=c++11
CFLAGS +=      -I${HOME}/libs/json-hpp
CFLAGS +=      -I$(PREFIX_BASEDIR_SRC)

# Enable debug output:
#CFLAGS +=      -D_DEBUG

TARGET	= $(LIB_TARGET)
all: $(TARGET)

$(LIB_TARGET): $(LIB_OBJS)
	$(RM) $@;
	$(AR) $(AR_FLAG) $@ $^
	$(RANLIB) $@

install:	all
		@mkdir -p $(INS_DIR_LIB)
		cp $(LIB_TARGET) $(INS_DIR_LIB)
		@mkdir -p $(INS_DIR_INC)
		cp $(PREFIX_BASEDIR_SRC)/come_1.h $(INS_DIR_INC)
		cp $(PREFIX_BASEDIR_SRC)/come.1.json.h $(INS_DIR_INC)

.c.o:
	$(CC) -c $(CFLAGS) $^ -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $^ -o $@

clean:
	$(RM) $(TARGET) $(LIB_OBJS) *.a *.cfbk
	find . -name "*.d" |xargs rm -f
	find . -name "*.cfbk" |xargs rm -f

