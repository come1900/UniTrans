#
# A example of Unit Test whth CppUnit
#
# you cppunit should config as: ./configure --prefix=`pwd`/inc --exec-prefix=`pwd`/bin 
# your cppunit path: tools/cppunit-1.12.1

EXEC = cppt
#add all your cpp files here
CPPS = main.cpp TestMan.cpp ../GeminiKeyStack.cpp ../GeminiKey.cpp

CXX = g++
PREFIX = ${HOME}/libs/cppunit-1.12.1-linux-x86-32

#INCLUDEDIR = ${PREFIX}/inc/include
# for cppunit-1.12.1
INCLUDEDIR = ${PREFIX}/include/
LDFLAGS = -L${PREFIX}/lib
#CXXFLAGS = -static
LDLIBS  = -lcppunit 
LDLIBS += -ldl  #if in cygwin, pls comment this


PLATFORM = linux-x86-32
CFLAGS+=-DLINUX
#add test data or point
CFLAGS+=-DUNIT_TEST
CFLAGS+=-DCPP_UNIT

EZLIBS_BASEDIR_LIBS=$(HOME)/libs

#use ezutil library for http_parse
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezutil
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezutil-$(PLATFORM)
#zlib compression
CFLAGS += -I${EZLIBS_BASEDIR_LIBS}/include/ezUtilCPP
LIBS += -L${EZLIBS_BASEDIR_LIBS}/lib -lezUtilCPP-$(PLATFORM) -lz-$(PLATFORM)

CFLAGS+= -I${EZLIBS_BASEDIR_LIBS}/openssl-${PLATFORM}/include
LIBS += -L${EZLIBS_BASEDIR_LIBS}/openssl-${PLATFORM}/lib -lcrypto
LIBS += -ldl

LDLIBS +=          -L$(HOME)/libs/lib -ljson-$(PLATFORM) $(LIBS)
CFLAGS += -I$(HOME)/libs/include/json

all : ${EXEC}
	@echo "Test result ->"
	@./${EXEC}

${EXEC} : ${CPPS}
	@echo "Making ..."
	${CXX} -I${INCLUDEDIR} ${LDFLAGS} ${CFLAGS} -o ${EXEC} ${CPPS} ${LDLIBS}

clean:
	rm -f ${EXEC} ${EXEC}.exe *.o *.cfbk
