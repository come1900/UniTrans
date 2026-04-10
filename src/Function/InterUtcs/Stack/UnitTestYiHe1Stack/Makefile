#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# branches.faceaip\ServiceManageSystem\src\Stack\UnitTestSmsStack\Makefile - Z:\svn\
#
# Copyright (C) 2013 ezlibs.com, All Rights Reserved.
#
# $Id: branches.faceaip\ServiceManageSystem\src\Stack\UnitTestSmsStack\Makefile 1 2013-12-02 09:36:07 WuJunjie Exp $
#
# explain
#      exp
#
# Update:
#    2013-12-02 09:36 WuJunjie Create
#
#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
#
# you cppunit should config as: ./configure --prefix=`pwd`/inc --exec-prefix=`pwd`/bin 
# your cppunit path: tools/cppunit-1.12.1

CPP_SOURCE = $(wildcard ../*.cpp)
H_SOURCE = $(wildcard ../*.h)

PLATFORM = linux-x86-32

#add all your cpp files here
CPPS = $(wildcard *.cpp) #main.cpp TestMan.cpp# ../YiHe1Stack.cpp
CPPS += $(CPP_SOURCE)

EXEC = cppt

CXX = g++
PREFIX = ${HOME}/libs/cppunit-1.12.1-linux-x86-32

#INCLUDEDIR = ${PREFIX}/inc/include
# for cppunit-1.12.1
INCLUDEDIR = ${PREFIX}/include/
LDFLAGS = -L${PREFIX}/lib
#CXXFLAGS = -static
LDLIBS  = -lcppunit 
LDLIBS += -ldl  #if in cygwin, pls comment this

LDLIBS +=          -L$(HOME)/libs/lib -ljson-$(PLATFORM)
CFLAGS += -I$(HOME)/libs/include/json

## tinyXML
LDLIBS +=          -L$(HOME)/libs/lib -ltinyXML-$(PLATFORM)
CFLAGS += -I$(HOME)/libs/include/tinyXML
CFLAGS += -DTIXML_USE_STL

all : ${EXEC}
	@echo ""
	@echo "Testing ... ->"
	@./${EXEC}

${EXEC} : ${CPPS} ${H_SOURCE}
	@echo "Making ..."
	${CXX} -I${INCLUDEDIR} ${LDFLAGS} ${CFLAGS} -o ${EXEC} ${CPPS} ${LDLIBS}

clean:
	rm -f ${EXEC} ${EXEC}.exe *.o *.cfbk
