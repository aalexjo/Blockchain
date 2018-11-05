#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include "client.hpp"

using namespace std;

static int wait_for_start = 1;

static void start(int signum) {
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

int main(int argc, char** argv) {

	//set signal handlers
	signal(SIGUSR1, start);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	//parse arguments, including membership
  assert(argc >= 2 + 3 + 1);
  int process_i = atoi(argv[1]);
  int process_n = atoi(argv[2]);
  int message_n = atoi(argv[argc-1]);
  assert(process_i <= process_n);
  assert(4 + (process_n)*3 == argc);

  vector <int> id;
  vector <string> ips;
  vector <string> ports;
  for (int i = 3; i < argc - 1; i+=3) {
    id.push_back(atoi(argv[i]));
    ips.push_back(argv[i+1]);
    ports.push_back(argv[i+2]);
  }

  /* Debug printing
  cout << "argc:" << argc << '\n';
  cout << "process number"<< ":" << "ip" << ":" << "port" << '\n';
  for (int i = 0; i < process_n; i++) {
    cout << id[i] << ":" << ips[i] << ":" << ports[i] << '\n';
  }
  // */

  Client client(process_i, process_n, message_n, id, ips, ports);
  client.display();

	//initialize application
	//start listening for incoming UDP packets
	printf("Initializing.\n");

	//wait until start signal
	while(wait_for_start) {
		struct timespec sleep_time;
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 1000;
		nanosleep(&sleep_time, NULL);
	}

	//broadcast messages
	printf("Broadcasting messages.\n");

	//wait until stopped
	while(1) {
		struct timespec sleep_time;
		sleep_time.tv_sec = 1;
		sleep_time.tv_nsec = 0;
		nanosleep(&sleep_time, NULL);
	}
}
