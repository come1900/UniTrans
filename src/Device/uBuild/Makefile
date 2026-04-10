# Copyright (C) 2012  WuJunjie(Joy.Woo@hotmail.com)

# platforms:

PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-x86-64-centos5
PLATFORM = linux-am335x

VERSION =	1.0
DIFF_VERSION =	0.9

DIST =		tLock-$(PLATFORM)

CFLAGS =	-Wall -g -O2 -MD
CFLAGS += -D_DEBUG #debugme

#include         Makefile.Defines.$(PLATFORM)
include         ./Makefile.Defines/Makefile.Defines.$(PLATFORM)

CFLAGS += -I$(HOME)/libs/include/ezThread
# libsigc
# CFLAGS += -I$(HOME)/libs/libsigc++-$(PLATFORM)/lib/sigc++-2.0/include
# CFLAGS += -I$(HOME)/libs/libsigc++-$(PLATFORM)/include/sigc++-2.0
CLEAN_FILES += *actual* *rewrite* a.txt

#特殊功能区
#

CPPFLAGS =	$(CFLAGS) 

#LIBS =          -L$(HOME)/libs/libsigc++-$(PLATFORM)/lib/ -lsigc-2.0
LIBS +=         -L$(HOME)/libs/lib -lezThread-$(PLATFORM) -lpthread

tLock-$(PLATFORM):tLock.o ../DevLock.o
	$(CPP) -o $@ $^ $(LIBS)

FILES =		*.h *.cpp Makefile*

tar:	clean
		tar -czf $(DIST).tgz $(FILES)

-include	*.d

CLEAN_FILES = $(DIST) *.o *.gdb *.d *.cfbk *.tgz

all: $(DIST)

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

clean: 
	rm -f $(CLEAN_FILES)
dbg: $(DIST)
	mv $(DIST) /home/share

