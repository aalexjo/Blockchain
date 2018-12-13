#pragma once
#include "perfectLink.hpp"
#include <vector>
#include <string>
#include <map>

typedef struct{
  PerfectLink* link;
  std::vector<msg_s> *received;
  std::vector<std::vector<std::vector<msg_s*>>> *ack;
  int pid;
  int n;
  pthread_mutex_t* received_lock;
  pthread_mutex_t* ack_lock;

} URBThreadList;

class reliableBroadcast{
public:
  reliableBroadcast(int n, int pid, std::vector<int> ports, int message_n);

  void broadcast(struct msg_s* msg);
  void receiver();
  msg_s* canDeliver(int pi_sender, int m);
  void urbPrint();
private:
	void pp2pCallback(struct msg_s* msg);

	int n; //num of Procsess
  int max_m;
  int pid;
	PerfectLink* link;
	unsigned int seq_nr;
  URBThreadList threadListItem;
  std::vector<msg_s> received;
	std::vector<std::vector<msg_s>> delivered;

  std::vector<std::vector<std::vector<msg_s*>>> ack; //use char since bool sucks

  pthread_mutex_t received_lock;
  pthread_mutex_t ack_lock;
};
