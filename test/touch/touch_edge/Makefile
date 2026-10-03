# Copyright (C) 2025  ezlibs.com
#
# touch_edge Makefile - Edge (边缘设备)
# 复用 src/Function/Touch/Edge 的正式代码
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

PROJECT_NAME=touch_edge
DEF_VERSION =	1.0.0
DIFF_VERSION =	0.0.0

include         ../../../build/Makefile.Defines/Makefile.Defines.$(PLATFORM)

CFLAGS += -Wall -g -O2 -MD
# CFLAGS += -std=c++11
CFLAGS += -D_DEBUG #debugme
CFLAGS += -D_USE_EZLOG_FILENAME
# wss TLS 编译开关：注释/删除此行可完全排除 wss，无需链接 OpenSSL
CFLAGS += -D_FUNC_TouchEdge_EnableTls

##############################################################################
# 代码文件位置 - 复用正式代码，但在本地编译
TOUCH_EDGE_BASEDIR_SRC=../../../src/Function/Touch/Edge
TOUCH_EDGE_LOCAL_OBJDIR=./obj

# 确保对象目录存在
$(shell mkdir -p $(TOUCH_EDGE_LOCAL_OBJDIR))

##############################################################################
#库文件位置
#使用发布库
EZLIBS_BASEDIR_LIBS=$(HOME)/libs

# 正式代码的对象文件在本地构建
OBJS_DEVWS = $(TOUCH_EDGE_LOCAL_OBJDIR)/DevWsRegisterCli.o
OBJS_FUNCREG = $(TOUCH_EDGE_LOCAL_OBJDIR)/FunRegisterCli.o
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

CFLAGS += -I$(TOUCH_EDGE_BASEDIR_SRC)
CFLAGS += -I./

# OpenSSL 静态库（wss TLS 支持，见 spec-design-tls-websocket.md「编译与链接」）
# 仅当定义了 -D_FUNC_TouchEdge_EnableTls 才链接；否则完全排除 wss
ifneq ($(filter -D_FUNC_TouchEdge_EnableTls,$(CFLAGS)),)
OPENSSL_INSTALL_PREFIX = $(HOME)/libs/openssl-linux
CFLAGS += -I$(OPENSSL_INSTALL_PREFIX)/include -I$(HOME)/libs/zlib-linux/include
LIBS += $(OPENSSL_INSTALL_PREFIX)/lib64/libssl.a \
        $(OPENSSL_INSTALL_PREFIX)/lib64/libcrypto.a \
        $(HOME)/libs/zlib-linux/lib/libz.a
endif

LIBS += -lpthread
LIBS += -lrt
LIBS += -ldl
LIBS += -lm

# Edge 程序
OBJS_TOUCH_EDGE = $(OBJS_DEVWS) $(OBJS_FUNCREG) $(OBJS_MAIN)
DIST_TOUCH_EDGE = touch_edge-$(PLATFORM)

DIST = $(DIST_TOUCH_EDGE)

all: $(DIST)

$(DIST_TOUCH_EDGE):$(OBJS_TOUCH_EDGE)
	$(CPP) -o $@ $^ $(LIBS)

# 编译正式代码的源文件到本地对象目录
$(TOUCH_EDGE_LOCAL_OBJDIR)/DevWsRegisterCli.o: $(TOUCH_EDGE_BASEDIR_SRC)/DevWsRegisterCli.cpp $(TOUCH_EDGE_BASEDIR_SRC)/DevWsRegisterCli.h
	$(CPP) -c $(CFLAGS) $< -o $@

$(TOUCH_EDGE_LOCAL_OBJDIR)/FunRegisterCli.o: $(TOUCH_EDGE_BASEDIR_SRC)/FunRegisterCli.cpp $(TOUCH_EDGE_BASEDIR_SRC)/FunRegisterCli.h
	$(CPP) -c $(CFLAGS) $< -o $@

# 编译 Main.cpp
./Main.o: ./Main.cpp
	$(CPP) -c $(CFLAGS) $< -o $@

tar:	clean
	tar -czf $(PROJECT_NAME).tgz *.h *.cpp Makefile*

-include *.d
-include $(TOUCH_EDGE_LOCAL_OBJDIR)/*.d

CLEAN_FILES = $(DIST) *.o *.gdb *.d *.cfbk *.tgz
CLEAN_FILES += $(TOUCH_EDGE_LOCAL_OBJDIR)/*.o $(TOUCH_EDGE_LOCAL_OBJDIR)/*.d

clean:
	rm -f $(CLEAN_FILES)
	rm -rf $(TOUCH_EDGE_LOCAL_OBJDIR)
	find . -name "*.d" | xargs rm -f 2>/dev/null || true
	rm -f *.log
