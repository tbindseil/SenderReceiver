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

all: sender receiver

sender.o: sender.cpp sender.h
	$(CXX) $(CXXFLAGS) -c -I$(INC) sender.cpp

sender: sender.o draw_cmd.o socket_wrapper.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o sender sender.o draw_cmd.o socket_wrapper.o

receiver.o: receiver.cpp receiver.h
	$(CXX) $(CXXFLAGS) -c -I$(INC) receiver.cpp

receiver: receiver.o socket_wrapper.o
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o receiver receiver.o socket_wrapper.o

draw_cmd.o: draw_cmd.cpp draw_cmd.h
	$(CXX) $(CXXFLAGS) -c -I$(INC) draw_cmd.cpp

socket_wrapper.o: socket_wrapper.cpp socket_wrapper.h
	$(CXX) $(CXXFLAGS) -c -I$(INC) socket_wrapper.cpp

clean:
	$(RM) sender.o sender draw_cmd.o receiver.o receiver socket_wrapper.o
