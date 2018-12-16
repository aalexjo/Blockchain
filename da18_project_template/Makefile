all: da_proc

da_proc: da_proc.cpp UDP.cpp perfectLink.cpp reliableBroadcast.cpp FIFObroadcast.cpp causalBroadcast.cpp
	g++ -std=c++11 -Wall -g -o da_proc da_proc.cpp UDP.cpp perfectLink.cpp reliableBroadcast.cpp FIFObroadcast.cpp causalBroadcast.cpp -lpthread


clean:
	rm da_proc
