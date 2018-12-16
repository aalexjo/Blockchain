all: da_proc

da_proc: da_proc.cpp
	g++ -std=c++11 -Wall -g -o da_proc da_proc.cpp udp.hpp p2pp.hpp -lpthread

clean:
	rm da_proc
