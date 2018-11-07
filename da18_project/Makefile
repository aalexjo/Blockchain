all: da_proc

da_proc: da_proc.cpp client.cpp
	g++ -std=c++11 -Wall -g -o da_proc da_proc.cpp client.cpp -lpthread

clean:
	rm da_proc
