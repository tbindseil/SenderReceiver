CC=gcc
CXX=g++
LD=ld

RM=rm -f

ALLFLAGS=-g -Wall

CFLAGS=$(ALLFLAGS)
CPPFLAGS=$(ALLFLAGS)
CXXFLAGS=$(ALLFLAGS)
LDFLAGS=$(ALLFLAGS)

LDFLAGS=-pthread

INC=./

all: sender receiver draw_cmd.o

sender.o: sender.cpp sender.h
	$(CXX) $(CXXFLAGS) -c -I$(INC) sender.cpp

sender: sender.o draw_cmd.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o sender sender.o draw_cmd.o

receiver.o: receiver.cpp receiver.h
	$(CXX) $(CXXFLAGS) -c -I$(INC) receiver.cpp

receiver: receiver.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o receiver receiver.o

draw_cmd.o: draw_cmd.cpp draw_cmd.h
	$(CXX) $(CXXFLAGS) -c -I$(INC) draw_cmd.cpp

clean:
	$(RM) sender.o sender draw_cmd.o receiver.o receiver
