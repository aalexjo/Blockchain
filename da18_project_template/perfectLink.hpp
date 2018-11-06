#include "UDP.hpp"
#include <vector>
#include <string>
#include <set>

typedef void (*perfectLinkMessageCallback)(struct msg_s* msg);
// const char *ip, char *data, int datalength ^

void error(char const *e);

typedef struct{
  UDP udp;
  msg_s *msg;
} perfectLinkThreadList;

class PerfectLink{
public:
  PerfectLink(const char* addr, int port, perfectLinkMessageCallback callback);
  void broadcast(struct msg_s* msg);
  void startReceiving();
private:
  //std::vector<std::string> delivered;
  std::vector<perfectLinkThreadList> delivered;
  std::string processID;
  //static void udpcallback(struct msg_s* msg);
  UDP udp;
};
