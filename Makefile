CXX=g++

CXXFLAGS = -Wall -g -std=c++11
LDFLAGS = -lpthread

OBJS = sws.o

all: a1

a1: $(OBJS)
	# $(CXX) $(CXXFLAGS) -o sws sws.cpp $(LDFLAGS) -lreadline.so -lhistory -ltermcap
	$(CXX) $(CXXFLAGS) -o sws sws.cpp $(LDFLAGS)

clean: 
	rm -rf $(OBJS) sws 
