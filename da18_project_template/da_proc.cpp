#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include "reliableBroadcast.hpp"
#include "perfectLink.hpp"
#include "FIFObroadcast.hpp"
#include "causalBroadcast.hpp"
using namespace std;

FIFObroadcast* fifo;
causalBroadcast* CB;

static int wait_for_start = 1;
//static int pNumber, msgNum;
//char  membership;
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

	CB->printOutput();

	//exit directly from signal handler
	exit(0);
}

int main(int argc, char** argv) {
	printf("Initializing.\n");

	//set signal handlers
	signal(SIGUSR1, start);
	signal(SIGTERM, stop);
	signal(SIGINT, stop);

	//parse arguments, including membership
  assert(argc >= 3);
  int pid = atoi(argv[1]);
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
	vector<bool> dependencies(process_n, false);//TODO: probably better to init as false

  // Get ports
  for(int i = 0; i < process_n; i++) {
    membership >> id >> ip >> port;
    ids.push_back(id);
    ips.push_back(ip);
    ports.push_back(port);
    //process_s process = { id, ip, port };
    //processes.push_back(process);
  }
  membership.ignore();

  // Get dependencies
  string line;
  for(int i = 0; i < process_n; i++) {
    getline(membership, line); // Get line till matches current pid
    if(i == pid) {
      cout << "myline:" << line << '\n';
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


	//printf("port %d, pid: %d", ports[pid],pid);

	//PerfectLink perfectLink(pid, ports, perfectLinkTestCallback);
	//perfectLink.startReceiving();
	fifo = new FIFObroadcast(process_n, pid-1, ports, message_n);

	CB = new causalBroadcast(process_n, pid-1, ports, message_n, dependencies);
	CB->startReceiving();


	//wait until start signal
	while(wait_for_start) {
		struct timespec sleep_time;
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 1000;
		nanosleep(&sleep_time, NULL);
	}

	struct msg_s msg = {0, pid-1, 0, 0, new int[process_n]()};
	//bzero(&msg, sizeof(msg));
	msg.seq_nr = 0;
	msg.sender = pid-1;
	msg.is_ack = 0;
	msg.ack_from = 0;

	//broadcast messages
	printf("Broadcasting messages.\n");

	//wait until stopped
	while(msg.seq_nr < message_n) {
		msg.seq_nr = msg.seq_nr + 1;
		CB->broadcast(&msg);
	}
	CB->stopReceiving();
	printf("all done in p%d\n",pid);
}
