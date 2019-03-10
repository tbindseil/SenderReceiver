CC=gcc
CXX=g++
LD=ld

RM=rm -f

ALLFLAGS=-g -Wall

CFLAGS=$(ALLFLAGS)
CPPFLAGS=$(ALLFLAGS)
CXXFLAGS=$(ALLFLAGS)
LDFLAGS=$(ALLFLAGS)

LDLIBS=

INC=./

all: sender receiver

sender.o: sender.cpp sender.h
	$(CXX) $(CXXFLAGS) -c -I$(INC) sender.cpp

sender: sender.o
	$(CXX) $(CXXFLAGS) -o sender sender.o

receiver.o: receiver.cpp receiver.h
	$(CXX) $(CXXFLAGS) -c -I$(INC) receiver.cpp

receiver: receiver.o
	$(CXX) $(CXXFLAGS) -o receiver receiver.o

clean:
	$(RM) sender.o sender receiver.o receiver
