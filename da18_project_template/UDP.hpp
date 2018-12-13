//kanskje noen headergaurds
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <mutex>
typedef void (*UDPMessageCallback)(struct msg_s* msg);//const char *ip, char *data, int datalength);
//int piders;
typedef struct{
  int port;
  int n;
  std::function<void(msg_s*)> callback;
} UDPThreadList;

struct msg_s{
  long seq_nr;
  long sender;
  long is_ack;
  long ack_from;
  //int* VC;
};

class UDP{
public:
  UDP(int pid, std::vector<int> ports, std::function<void(msg_s*)> callback);
  void broadcast(struct msg_s* msg);//char const * data, int dataLength);
  void startReceiving();
  void udpPrint();


private:
  std::mutex broadcast_mutex;
  int soc;
  int n;
  //void *thr_listener(void* arg);
  UDPThreadList threadListItem;
  //const char* ipaddr;
  std::vector<int> ports;
  int pid;
  int broadcast_count;

};
