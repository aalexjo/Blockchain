//kanskje noen headergaurds
#pragma once
#include <string>

using namespace std;

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
  UDP(const char* ipaddr, int port, UDPMessageCallback callback);
  void broadcast(struct msg_s* msg);//char const * data, int dataLength);
  void startReceiving();


private:
  //void *thr_listener(void* arg);
  UDPThreadList threadListItem;
  const char* ipaddr;
};
