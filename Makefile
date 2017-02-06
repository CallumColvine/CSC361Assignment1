CXX=g++

CXXFLAGS = -Wall -g -std=c++11
LDFLAGS =  

OBJS = sws.o

all: a1

a1: $(OBJS)
	$(CXX) $(CXXFLAGS) -o sws sws.cpp $(LDFLAGS) -lreadline -lhistory -ltermcap

clean: 
	rm -rf $(OBJS) sws 
