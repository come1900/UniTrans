# Copyright (C) 2026  ezlibs.com
#
# touch_ca_export Makefile - 导出 libezsocket 内置 CA 公钥证书到 PEM 文件
# 供 Python touch_manager 以 INGRESS_CA_FILE 信任 ingress 的 wss 证书。
#
# platforms:
PLATFORM = linux

PROJECT_NAME=touch_ca_export
DEF_VERSION =	1.0.0
DIFF_VERSION =	0.0.0

include         ../../../build/Makefile.Defines/Makefile.Defines.$(PLATFORM)

CFLAGS += -Wall -g -O2

##############################################################################
# 库文件位置
EZLIBS_BASEDIR_LIBS=$(HOME)/libs

# libezsocket 头文件与静态库（用发布库，含 ez_wss TLS 支持与全部内部符号）
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezsocket
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezsocket-$(PLATFORM)

# ezutil 提供 http_parser / websocket parser / SHA1 / base64 等内部符号
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)

# OpenSSL 静态库（wss TLS 支持，见 spec-design-tls-websocket.md「编译与链接」）
OPENSSL_INSTALL_PREFIX = $(HOME)/libs/openssl-linux
CFLAGS += -I$(OPENSSL_INSTALL_PREFIX)/include -I$(HOME)/libs/zlib-linux/include
LIBS += $(OPENSSL_INSTALL_PREFIX)/lib64/libssl.a \
        $(OPENSSL_INSTALL_PREFIX)/lib64/libcrypto.a \
        $(HOME)/libs/zlib-linux/lib/libz.a

LIBS += -lpthread
LIBS += -lrt
LIBS += -ldl
LIBS += -lm

OBJS = ./main.o
DIST_TOUCH_CA_EXPORT = touch_ca_export-$(PLATFORM)

DIST = $(DIST_TOUCH_CA_EXPORT)

all: $(DIST)

$(DIST_TOUCH_CA_EXPORT):$(OBJS)
	$(CPP) -o $@ $^ $(LIBS)

./main.o: ./main.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(DIST) *.o *.d *.gdb
