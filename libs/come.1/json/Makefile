# Makefile for come.1.json tutorial (JSON implementation)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
INCLUDES = -I${HOME}/libs/json-hpp -I..
LDFLAGS = 

TARGET = ut-come.1.json
SOURCES = come.1.json.cpp ut-come.1.json.cpp
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean utest

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

utest: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

