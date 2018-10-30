#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "perfectLink.hpp"

static int wait_for_start = 0;

static void start(int signum) {
	printf("Starting.\n");
	wait_for_start = 0;
}

static void stop(int signum) {
	//reset signal handlers to default
	signal(SIGTERM, SIG_DFL);
	signal(SIGINT, SIG_DFL);

	//immediately stop network packet processing
	printf("Immediately stopping network packet processing.\n");

	//write/flush output file if necessary
	printf("Writing output.\n");

	//exit directly from signal handler
	exit(0);
}

void perfectLinkTestCallback(const char * ip, char * data, int datalength) {
	// Assuming an ascii string here - a binary blob (including '0's) will
  // be ugly/truncated.
	// printf("Received perfectLink message from %s: '%s'\n", ip, data);
	printf("Perfect link recived");
	// return std::make_tuple()
}

int main(int argc, char** argv) {

	//set signal handlers
	signal(SIGUSR1, start);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	char const *addrR = "127.0.0.1";
	int portR = 1729;
	//parse arguments, including membership
	//initialize application
	//start listening for incoming UDP packets
	printf("Initializing.\n");
	//member file
	//buffer
	perfectLink perfectLinkReceive(addrR, portR, perfectLinkTestCallback);
	perfectLinkReceive.startReceiving();

	//wait until start signal
	while(wait_for_start) {
		struct timespec sleep_time;
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 1000;
		nanosleep(&sleep_time, NULL);
	}


	//broadcast messages
	printf("Broadcasting messages.\n");

	char const *addrB = "127.0.0.1";
	int portB = 1729;

	perfectLink perfectLinkBroadcast(addrB, portB, perfectLinkTestCallback);

	//wait until stopped
	perfectLinkBroadcast.broadcast("hallo all", 10);
	struct timespec sleep_time;
	sleep_time.tv_sec = 1;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time, NULL);
}
