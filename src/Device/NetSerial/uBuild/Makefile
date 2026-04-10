# Copyright (C) 2012  ezlibs.com
#
# platforms:
PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-x86-64-centos5
#PLATFORM = linux-am335x

VERSION =	1.3
DIFF_VERSION =	1.0

PROJECT_NAME=netSerial

include         Makefile.Defines.$(PLATFORM)

#CFLAGS =	-Wall -g -O2 -MD
#CFLAGS += -D_DEBUG #debugme

CFLAGS += -Wno-write-strings

CFLAGS += -D_UNIT_TEST_VER

##############################################################################
# 代码文件位置
NetSerial_BASEDIR_SRC=../src

##############################################################################
#库文件位置
#使用自带库
# libezThread libppSockets
#EZLIBS_BASEDIR_LIBS=../libs

##使用发布库
EZLIBS_BASEDIR_LIBS=$(HOME)/libs

OBJS_NetSerial = \
		$(NetSerial_BASEDIR_SRC)/NetSerialLog.o \
		$(NetSerial_BASEDIR_SRC)/NetSerialHandler.o\
		$(NetSerial_BASEDIR_SRC)/NetSerialTcpSocket.o \
		$(NetSerial_BASEDIR_SRC)/NetSerial.o


#
CFLAGS += -DEXAMPLE_NetSerialTCPSOCKETCONNECTOR #tcp客户端
OBJS_NetSerial += \
		$(NetSerial_BASEDIR_SRC)/NetSerialTcpSocketConnector.o

#
CFLAGS += -DEXAMPLE_SIGNAL_ONDATA #使用数据回调机制
#

#use ezutil library for http_parse
#CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
#LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)
#ezUtilCPP
#CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/ezUtilCPP/
#LIBS +=	-L$(EZLIBS_BASEDIR_LIBS)/lib -lezUtilCPP-$(PLATFORM)

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ppSockets
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lppSockets-$(PLATFORM)

## json
#CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/json
#LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -ljson-$(PLATFORM)
#zlib
#CFLAGS += -I$(EZLIBS_BASEDIR_LIBS)/include/zlib
#LIBS += -L$(EZLIBS_BASEDIR_LIBS)/lib -lz-$(PLATFORM)

#
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezThread
LIBS +=	-L${EZLIBS_BASEDIR_LIBS}/lib -lezThread-$(PLATFORM)
LIBS +=	-lpthread

#多线程，用ezthread库
OBJS_MULTITHREAD_P = $(NetSerial_BASEDIR_SRC)/main_multi.o
OBJS_MULTITHREAD_P += $(OBJS_NetSerial)
OBJS_MULTITHREAD_P += $(OBJS_CONFIGS)
DIST_MULTITHREAD = $(PROJECT_NAME)-$(PLATFORM)

DIST =	$(DIST_MULTITHREAD)

all: $(DIST)


$(DIST_MULTITHREAD):$(OBJS_MULTITHREAD_P)
		$(CPP) -o $@ $^ $(LIBS)

tar:	clean
		tar -czf $(PROJECT_NAME).tgz *.h *.cpp Makefile*

-include *.d

.c.o:
	$(CC) -c $(CFLAGS) $< -o $@

.cpp.o:
	$(CPP) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(PROJECT_NAME)* #$(DIST)
	find $(NetSerial_BASEDIR_SRC) -name "*.o" |xargs rm -f
	find $(NetSerial_BASEDIR_SRC) -name "*.d" |xargs rm -f
	find $(NetSerial_BASEDIR_SRC) -name "*.cfbk" |xargs rm -f

