# Copyright (C) 2025  ezlibs.com
#
# touch_ingress Makefile - Ingress Service (接入服务)
# 复用 src/Function/Touch/Ingress 的正式代码
#
# platforms:
PLATFORM = linux
#PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-x86-64-centos5
#PLATFORM = linux-am335x
# openwrt 2026
#PLATFORM = linux-mipsel-openwrt-linux
# cross 2026
#PLATFORM = linux-x86-64

PROJECT_NAME=touch_ingress
DEF_VERSION =	1.0.0
DIFF_VERSION =	0.0.0

include         ../../../build/Makefile.Defines/Makefile.Defines.$(PLATFORM)

CFLAGS += -Wall -g -O2 -MD
#CFLAGS += -static
CFLAGS += -D_DEBUG #debugme

##############################################################################
# 代码文件位置 - 复用正式代码，但在本地编译
TOUCH_INGRESS_BASEDIR_SRC=../../../src/Function/Touch/Ingress
TOUCH_INGRESS_LOCAL_OBJDIR=./obj

# 确保对象目录存在
$(shell mkdir -p $(TOUCH_INGRESS_LOCAL_OBJDIR))

##############################################################################
#库文件位置
#使用发布库
EZLIBS_BASEDIR_LIBS=$(HOME)/libs

# 正式代码的对象文件在本地构建
OBJS_DEVWS = $(TOUCH_INGRESS_LOCAL_OBJDIR)/DevWsRegisterSvr.o
OBJS_FUNCREG = $(TOUCH_INGRESS_LOCAL_OBJDIR)/FunRegisterSvr.o
OBJS_MAIN = ./Main.o

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezThread
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezThread-$(PLATFORM)

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezsocket
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezsocket-$(PLATFORM)

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)

CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/come1
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lcome1-$(PLATFORM)

CFLAGS += -I$(TOUCH_INGRESS_BASEDIR_SRC)
CFLAGS += -I./

LIBS += -lpthread
LIBS += -lrt
LIBS += -lm

# Ingress 程序
OBJS_TOUCH_INGRESS = $(OBJS_DEVWS) $(OBJS_FUNCREG) $(OBJS_MAIN)
DIST_TOUCH_INGRESS = touch_ingress-$(PLATFORM)

DIST = $(DIST_TOUCH_INGRESS)

all: $(DIST)

$(DIST_TOUCH_INGRESS):$(OBJS_TOUCH_INGRESS)
	$(CPP) -o $@ $^ $(LIBS)

# 编译正式代码的源文件到本地对象目录
$(TOUCH_INGRESS_LOCAL_OBJDIR)/DevWsRegisterSvr.o: $(TOUCH_INGRESS_BASEDIR_SRC)/DevWsRegisterSvr.cpp $(TOUCH_INGRESS_BASEDIR_SRC)/DevWsRegisterSvr.h
	$(CPP) -c $(CFLAGS) $< -o $@

$(TOUCH_INGRESS_LOCAL_OBJDIR)/FunRegisterSvr.o: $(TOUCH_INGRESS_BASEDIR_SRC)/FunRegisterSvr.cpp $(TOUCH_INGRESS_BASEDIR_SRC)/FunRegisterSvr.h
	$(CPP) -c $(CFLAGS) $< -o $@

# 编译 Main.cpp
./Main.o: ./Main.cpp
	$(CPP) -c $(CFLAGS) $< -o $@

tar:	clean
	tar -czf $(PROJECT_NAME).tgz *.h *.cpp Makefile*
dbg: $(DIST)
	scp $(DIST) root@47.100.49.48:/root/touch/

-include *.d
-include $(TOUCH_INGRESS_LOCAL_OBJDIR)/*.d

CLEAN_FILES = $(DIST) *.o *.gdb *.d *.cfbk *.tgz
CLEAN_FILES += $(TOUCH_INGRESS_LOCAL_OBJDIR)/*.o $(TOUCH_INGRESS_LOCAL_OBJDIR)/*.d

clean:
	rm -f $(CLEAN_FILES)
	rm -rf $(TOUCH_INGRESS_LOCAL_OBJDIR)
	find . -name "*.d" | xargs rm -f 2>/dev/null || true
