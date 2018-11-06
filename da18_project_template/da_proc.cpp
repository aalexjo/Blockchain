#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <cassert>
#include <fstream>
#include "reliableBroadcast.hpp"
#include "perfectLink.hpp"
#include "FIFObroadcast.hpp"
using namespace std;


static int wait_for_start = 1;
//static int pNumber, msgNum;
//char  membership;
static void start(int signum) {
	printf("Starting.\n");
	wait_for_start = 0;
}

void UDPTestCallback(struct msg_s* msg){//const char * ip, char * data, int datalength){

  // Assuming an ascii string here - a binary blob (including '0's) will
  // be ugly/truncated.
  printf("Received UDP message from %d with seq number '%d'\n",msg->sender, msg->seq_nr);

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

void perfectLinkTestCallback(struct msg_s* msg) {
	// Assuming an ascii string here - a binary blob (including '0's) will
  // be ugly/truncated.
	// printf("Received perfectLink message from %s: '%s'\n", ip, data);
	printf("Perfect link recived from %d with seq_nr: %d  \n" , msg->sender, msg->seq_nr);
	// return std::make_tuple()
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
  while(membership >> id >> ip >> port) {
    ids.push_back(id);
    ips.push_back(ip);
    ports.push_back(port);
  }
	membership.close();

	//initialize application


	//printf("port %d, pid: %d", ports[pid],pid);

	//PerfectLink perfectLink(pid, ports, perfectLinkTestCallback);
	//perfectLink.startReceiving();
	FIFObroadcast fifo(process_n, pid-1, ports);
	fifo.startReceiving();


	//wait until start signal
	while(wait_for_start) {
		struct timespec sleep_time;
		sleep_time.tv_sec = 0;
		sleep_time.tv_nsec = 1000;
		nanosleep(&sleep_time, NULL);
	}

	struct msg_s msg;
	//bzero(&msg, sizeof(msg));
	msg.seq_nr = 0;
	msg.sender = pid-1;
	msg.is_ack = 1;
	msg.ack_from = 0;

	//broadcast messages
	printf("Broadcasting messages.\n");

	//wait until stopped
	while(msg.seq_nr < message_n) {
		msg.seq_nr = msg.seq_nr + 1;
		//perfectLink.broadcast(&msg);//"hallo all",10 );
		fifo.broadcast(&msg);

	}
	struct timespec sleep_time;
	sleep_time.tv_sec = 2;
	sleep_time.tv_nsec = 0;
	nanosleep(&sleep_time, NULL);
}
