#include "UDP.hpp"
#include <vector>
#include <string>
#include <set>

typedef void (*perfectLinkMessageCallback)(const char *ip, char *data, int datalength);

void error(char const *e);

typedef struct{
  UDP udp;
  char const *data;
  int datalength;
} perfectLinkThreadList;

class perfectLink{
public:
  perfectLink(const char* addr, int port, perfectLinkMessageCallback callback);
  void broadcast(const char* data, int datalength);
  void startReceiving();
private:
  //std::vector<std::string> delivered;
  std::set<char> delivered;
  int messagesSent;
  std::string processID;
  UDP udp;
  UDPMessageTestCallback udpcallback;
};
