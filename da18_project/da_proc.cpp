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
#include <fstream>
#include <thread>
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
  //assert(argc == 4);
  int process_i = atoi(argv[1]) - 1;
  char* file_name = argv[2];
  int message_n = atoi(argv[3]);

  //parse membership
  fstream  membership;
  int id, port;
  string ip;

  int process_n;
  vector <int> ids;
  vector <string> ips;
  vector <int> ports;
  membership.open(file_name, ios::in);
  membership >> process_n;
  while(membership >> id >> ip >> port) {
    ids.push_back(id);
    ips.push_back(ip);
    ports.push_back(port);
  }
  membership.close();

	//initialize application
  Client client(process_i, process_n, message_n, ids, ips, ports);
  //client.display();

	//start listening for incoming UDP packets
	printf("Initializing.\n");
  thread receiveMsgs(&Client::startReceiving, &client);

	//wait until start signal
	while(wait_for_start) {
		struct timespec sleep_time;
		sleep_time.tv_sec = 2;
		sleep_time.tv_nsec = 1000;
		nanosleep(&sleep_time, NULL);
	}

	//broadcast messages
	printf("Broadcasting messages.\n");
  client.broadcastMessages();

	//wait until stopped
	while(1) {
		struct timespec sleep_time;
		sleep_time.tv_sec = 1;
		sleep_time.tv_nsec = 0;
		nanosleep(&sleep_time, NULL);
	}
}
