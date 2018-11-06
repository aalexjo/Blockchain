#pragma once
#include "perfectLink.hpp"
#include <vector>
#include <string>
#include <map>

class reliableBroadcast{
public:
  reliableBroadcast(int n, int pid, std::vector<int> ports);

  void broadcast(struct msg_s* msg);
  void receiver();
  bool canDeliver(int pi_sender, int m);

private:
	void pp2pCallback(struct msg_s* msg);
  msg_s* ack_msg;

	int n; //num of Procsess
  int max_m;
  int pid;
	PerfectLink* link;
	unsigned int seq_nr;
	std::vector<msg_s> delivered;
	std::vector<std::vector<int>> forward;
	std::vector<std::map<int, std::vector<int>>> ack; //ack[src_pi][seq_nr][acking_pi]
};
