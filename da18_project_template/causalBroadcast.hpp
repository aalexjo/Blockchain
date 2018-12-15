#pragma once

#include "FIFObroadcast.hpp"
#include <vector>

typedef struct{
  reliableBroadcast *URB;
  int n;
  int pid;
  std::vector<int>* delivered;
  std::vector<std::string>* output;
  pthread_mutex_t* output_lock;
  pthread_mutex_t* VC_lock;
  int* VC;
  std::vector<bool> dependencies;
}CBThreadList;


class causalBroadcast{
public:
  causalBroadcast(int n, int pid, std::vector<int> ports, int message_n, std::vector<bool> dependencies);

  void broadcast(struct msg_s* msg);
  void startReceiving();
  void stopReceiving();
  void printOutput();

private:
  reliableBroadcast *URB;
  CBThreadList threadListItem;
  std::vector<bool> dependencies;
  int n;
  int pid;
  int message_n;
  int* VC;
  std::vector<msg_s> pending;
  pthread_t receiver;
  std::vector<int> delivered;

  FILE * fout;
  std::vector<std::string> output;
  pthread_mutex_t output_lock;
  pthread_mutex_t VC_lock;

};
