#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# Makefile - Z:\Dropbox\stock\trunk\code\opensource\openssl\tutorial_openssl\
#
# Copyright (C) 2013 ezlibs.com, All Rights Reserved.
#
# $Id: Makefile 1 2013-08-21 04:39:35 WuJunjie Exp $
#
# explain
#      exp
#
# Update:
#    2013-08-21 04:39 WuJunjie Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

# platforms:

PLATFORM = linux
#PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-x86-64-centos5
#PLATFORM = linux-am335x
#PLATFORM = linux-mips-openwrt

VERSION =	1.0
DIFF_VERSION =	0.9

DIST = geminiKeyCreator-$(PLATFORM) geminiKeyPerformance-$(PLATFORM) geminiLock-$(PLATFORM)

CFLAGS =	-Wall -g -O2 -MD
CFLAGS += -D_DEBUG #debugme

#include         Makefile.Defines.$(PLATFORM)
include		./Makefile.Defines/Makefile.Defines.$(PLATFORM)

CLEAN_FILES += *actual* *rewrite* a.txt

EZLIBS_BASEDIR_LIBS=$(HOME)/libs
GeminiLock_BASEDIR_SRC=..

OBJS_GeminiKey=\
	$(GeminiLock_BASEDIR_SRC)/GeminiKey.o \
	$(GeminiLock_BASEDIR_SRC)/GeminiKeyStack.o

OBJS_GeminiKeyCreator=\
	GeminiKeyCreator.o
OBJS_GeminiKeyCreator +=$(OBJS_GeminiKey)

OBJS_GeminiKeyPerformance=\
	GeminiKeyPerformance.o
OBJS_GeminiKeyPerformance +=$(OBJS_GeminiKey)

OBJS_GeminiLock=\
	GeminiLock.o
OBJS_GeminiLock +=$(OBJS_GeminiKey)

CFLAGS += -I$(GeminiLock_BASEDIR_SRC)

#use ezutil library for http_parse
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/zlib
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezUtilCPP
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezUtilCPP-$(PLATFORM) -lz-$(PLATFORM)
CFLAGS += -I$(HOME)/libs/include/json
LIBS +=   -L$(HOME)/libs/lib -ljson-$(PLATFORM)
CFLAGS += -I$(HOME)/libs/include/sqlite
LIBS +=    -L$(HOME)/libs/lib -lsqlite-$(PLATFORM)

CFLAGS+=-I${EZLIBS_BASEDIR_LIBS}/openssl-${PLATFORM}/include
LIBS += -L${EZLIBS_BASEDIR_LIBS}/openssl-${PLATFORM}/lib -lcrypto
LIBS += -ldl -lpthread

CPPFLAGS =	$(CFLAGS) 

all: $(DIST)

geminiKeyCreator-$(PLATFORM):$(OBJS_GeminiKeyCreator)
	$(CPP) -o $@ $^ $(LIBS)

geminiKeyPerformance-$(PLATFORM):$(OBJS_GeminiKeyPerformance)
	$(CPP) -o $@ $^ $(LIBS)

geminiLock-$(PLATFORM):$(OBJS_GeminiLock)
	$(CPP) -o $@ $^ $(LIBS)

FILES =		*.h *.cpp Makefile*

tar:	clean
		tar -czf $(DIST).tgz $(FILES)
dbg:$(DIST)
	mv $(DIST) /home/share
run:$(DIST)
	./$(DIST)

-include	*.d

CLEAN_FILES = $(DIST)

.c.o:
	$(CPP) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

clean: 
	rm -f $(CLEAN_FILES) *.db3 geminiKeyCreator-* geminiKeyPerformance-* geminiLock-*
	find $(GeminiLock_BASEDIR_SRC) -name "*.cfbk" | xargs rm -f
	find $(GeminiLock_BASEDIR_SRC) -name "*.gdb" | xargs rm -f
	find $(GeminiLock_BASEDIR_SRC) -name "*.d" | xargs rm -f
	find $(GeminiLock_BASEDIR_SRC) -name "*.o" | xargs rm -f

