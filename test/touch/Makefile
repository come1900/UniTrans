# Copyright (C) 2013  ezlibs.com
#
# Makefile - Two components: touch_edge, touch_ingress
# 复用 src/Function/Touch/ 下的正式代码
#
# platforms:
PLATFORM = linux
#PLATFORM = linux-x86-32
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = linux-x86-64-centos5
#PLATFORM = linux-am335x
# cross 2026
#PLATFORM = linux-x86-64

PROJECT_NAME=touch
DEF_VERSION =	1.0.0
DIFF_VERSION =	0.0.0

include         ../../build/Makefile.Defines/Makefile.Defines.$(PLATFORM)

CFLAGS += -Wno-write-strings
CFLAGS += -std=c++11

##############################################################################
# 两个组件独立编译
##############################################################################

all: touch_edge touch_ingress

touch_edge:
	@echo "Building touch_edge..."
	$(MAKE) -C touch_edge

touch_ingress:
	@echo "Building touch_ingress..."
	$(MAKE) -C touch_ingress

clean:
	@echo "Cleaning touch_edge..."
	$(MAKE) -C touch_edge clean
	@echo "Cleaning touch_ingress..."
	$(MAKE) -C touch_ingress clean

.PHONY: all touch_edge touch_ingress clean
