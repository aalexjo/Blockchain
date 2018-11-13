#pragma once
//#include "reliableBroadcast.hpp"
#include "UDP.hpp"

#include <vector>
#include <string>
#include <set>
#include <functional>
typedef void (*perfectLinkMessageCallback)(struct msg_s* msg);
// const char *ip, char *data, int datalength ^

void error(char const *e);

typedef struct{
  UDP* udp;
  msg_s *msg;
} perfectLinkThreadList;


class PerfectLink{
public:
  PerfectLink(int pid, std::vector<int> ports, std::function<void(msg_s*)> callback);
  void broadcast(struct msg_s* msg);
  void startReceiving();

private:
  static void UDPcallback(struct msg_s* msg);
  UDP* udp;
};
