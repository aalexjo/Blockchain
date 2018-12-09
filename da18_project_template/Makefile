all: da_proc

da_proc: da_proc.cpp UDP.cpp perfectLink.cpp reliableBroadcast.cpp FIFObroadcast.cpp
	g++ -Wall -g -o da_proc da_proc.cpp UDP.cpp perfectLink.cpp reliableBroadcast.cpp FIFObroadcast.cpp -lpthread -pg


clean:
	rm da_proc
