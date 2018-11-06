//kanskje noen headergaurds
#pragma once
#include <string>
#include <vector>

typedef void (*UDPMessageCallback)(struct msg_s* msg);//const char *ip, char *data, int datalength);

typedef struct{
  int port;
  UDPMessageCallback callback;
} UDPThreadList;

struct msg_s{
  int seq_nr;
  int sender;
  bool is_ack;
  int ack_from;
  //string msg;
};

class UDP{
public:
  UDP(int pid, std::vector<int> ports, UDPMessageCallback callback);
  void broadcast(struct msg_s* msg);//char const * data, int dataLength);
  void startReceiving();


private:
  //void *thr_listener(void* arg);
  UDPThreadList threadListItem;
  //const char* ipaddr;
  std::vector<int> ports;
};
