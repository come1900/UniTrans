# Makefile for come.1.json tutorial (JSON implementation)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
INCLUDES = -I${HOME}/libs/json-hpp -I.. -I../src
LDFLAGS =

TARGET = ut-come.1.json
# Source files: come.1.json.cpp is in ../src, test file is local
SOURCES = ../src/come.1.json.cpp ut-come.1.json.cpp
OBJECTS = come.1.json.o ut-come.1.json.o

.PHONY: all clean utest

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# come.1.json.o comes from ../src/come.1.json.cpp
come.1.json.o: ../src/come.1.json.cpp ../src/come.1.json.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# ut-come.1.json.o is local
ut-come.1.json.o: ut-come.1.json.cpp ../src/come.1.json.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

utest: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

