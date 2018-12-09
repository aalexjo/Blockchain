#pragma once
#include "perfectLink.hpp"
#include <vector>
#include <string>
#include <map>

typedef struct{
  PerfectLink* link;
  std::vector<msg_s> *received;
  std::vector<std::vector<std::vector<char>>> *ack;
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
  bool canDeliver(int pi_sender, int m);
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
	std::vector<msg_s> delivered;
	std::vector<std::vector<int>> forward;

  //the map data structure isrelativly slow, might want to do like git@prototype branch
	//std::vector<std::map<int, std::vector<int>>> ack; //ack[src_pi][seq_nr][acking_pi]
  std::vector<std::vector<std::vector<char>>> ack; //use char since bool sucks

  pthread_mutex_t received_lock;
  pthread_mutex_t ack_lock;
};
