PLATFORM = linux-x86-32
PLATFORM = linux
#PLATFORM = linux-x86-64
#PLATFORM = linux-hi3515
#PLATFORM = linux-arm
#PLATFORM = osx
#PLATFORM = linux-mips-openwrt

#CFLAGS =	-Wall -g -O2 -MD -static
include         ../../../../../library/Makefile.Defines/Makefile.Defines.$(PLATFORM)

CFLAGS += -Wall 
CFLAGS +=  -O2
CFLAGS += -D_FUNC_STANDALONE_UTIL#use main

#CFLAGS += -I$(HOME)/libs/include/ev
#LIBS =		-L$(HOME)/libs/lib -lev-$(PLATFORM) -lm #-lrt # -ldl -lm -lnsl -lrt -lpthread
CFLAGS += -I/home/wujj/libs/acl/include/ -I/home/wujj/libs/acl/include/acl
LIBS += -L/home/wujj/libs/acl/lib/linux64 -l_acl_cpp -l_protocol -l_acl

#ifeq ($(PLATFORM),macosx)
#else
	#LIBS += -lrt
#endif

LIBS += -lpthread

C_SRC = $(wildcard *.c)
C_OBJ = $(patsubst %c, %o, $(C_SRC))
CPP_SRC = $(wildcard *.cpp)
CPP_OBJ = $(patsubst %cpp, %o, $(CPP_SRC))

.PHONY:all clean

all:$(CPP_OBJ) $(C_OBJ)

.c.o:
	$(CC) $(CFLAGS) -o $@ $< $(LIBS) 
.cpp.o:
	$(CPP) $(CFLAGS) -o $@ $< $(LIBS)

clean:
	rm *~ *.o -f *.cfbk *.d
