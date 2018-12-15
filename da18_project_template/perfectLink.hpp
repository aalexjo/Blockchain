#pragma once
//#include "reliableBroadcast.hpp"
#include "UDP.hpp"

#include <vector>
#include <string>
#include <set>
#include <functional>
#include <map>
#include <utility>

typedef void (*perfectLinkMessageCallback)(struct msg_s* msg);
// const char *ip, char *data, int datalength ^

void error(char const *e);

typedef struct{
  UDP* udp;
  std::map<int, msg_s*> *broadcast_list;
  pthread_mutex_t *broadcast_lock;
  pthread_mutex_t *list_lock;
  int n;
} perfectLinkThreadList;


class PerfectLink{
public:
  PerfectLink(int pid, std::vector<int> ports, std::function<void(msg_s*)> callback);
  void broadcast(struct msg_s* msg);
  void startReceiving();
  void linkPrint();
  void stopBroadcast(int msg_seq_nr);

private:
  static void UDPcallback(struct msg_s* msg);
  UDP* udp;
  int n;

  std::map<int, msg_s*> broadcast_list;
  pthread_mutex_t broadcast_lock;
  pthread_mutex_t list_lock;

};
