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

ifndef PLATFORM
PLATFORM = linux
endif

EXEC = cppt
#add all your cpp files here
CPPS = main.cpp TestMan.cpp ../InterThings_gLockStack.cpp

CXX = g++
PREFIX = ${HOME}/libs/cppunit-1.12.1-${PLATFORM}

#INCLUDEDIR = ${PREFIX}/inc/include
# for cppunit-1.12.1
INCLUDEDIR = ${PREFIX}/include/
LDFLAGS = -L${PREFIX}/lib
#CXXFLAGS = -static
LDLIBS  = -lcppunit 
LDLIBS += -ldl  #if in cygwin, pls comment this


PLATFORM = linux
LDLIBS +=          -L$(HOME)/libs/lib -ljson-$(PLATFORM)
CFLAGS += -I$(HOME)/libs/include/json

all : ${EXEC}
	@echo "Test result ->"
	@./${EXEC}

${EXEC} : ${CPPS}
	@echo "Making ..."
	${CXX} -I${INCLUDEDIR} ${LDFLAGS} ${CFLAGS} -o ${EXEC} ${CPPS} ${LDLIBS}

clean:
	rm -f ${EXEC} ${EXEC}.exe *.o *.cfbk
