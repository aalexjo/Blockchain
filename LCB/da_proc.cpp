#include <stdio.h>
#include <sstream>
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

#include "p2pp.hpp"

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
  int pid = atoi(argv[1]) - 1;
  char* file_name = argv[2];
  int messageNbr= atoi(argv[3]);

  //parse membership
  fstream  membership;
  int id, port;
  string ip;

  int processNbr;
  vector <process_s> processes;
  membership.open(file_name, ios::in);
  membership >> processNbr;
  vector <bool> dependencies(processNbr, false);
  // Get id:ip:port
  for(int i = 0; i < processNbr; i++) {
    membership >> id >> ip >> port;
    process_s process = { id, ip, port };
    processes.push_back(process);
  }

  // Get dependencies
  string line;
  for(int i = 0; i < processNbr; i++) {
    getline(membership, line); // Get line till matches current pid
    if(i == pid) {
      stringstream pidLine(line);
      pidLine.ignore();
      int dep;
      while(pidLine >> dep) {
        dependencies[dep-1] = true;
      }
    }
  }
	membership.close();

	//initialize application
  P2PP p2pp(pid, processNbr, messageNbr, processes, nullptr);
  //client.display();

	//start listening for incoming UDP packets
	printf("Initializing.\n");
  thread receiveMsgs(&UDP::startReceiving, &p2pp);
  thread startSend(&P2PP::pp2pStartSend, &p2pp);

	//wait until start signal
	//while(wait_for_start) {
		struct timespec sleep_time;
		sleep_time.tv_sec = 2;
		sleep_time.tv_nsec = 1000;
		nanosleep(&sleep_time, NULL);
    //}

	//broadcast messages
	printf("Broadcasting messages.\n");
  for(int m = 0; m < messageNbr; m++) {
    for(int p = 0; p < processNbr; p++) {
      int* VC = (int*) malloc(sizeof(int) * processNbr);
      msg_s msg = {pid, pid, m, false, VC};
      p2pp.send(msg, p);
    }
  }

	//wait until stopped
	//while(1) {
  //struct timespec sleep_time;
		sleep_time.tv_sec = 10;
		sleep_time.tv_nsec = 0;
		nanosleep(&sleep_time, NULL);
    //}
}
