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
  std::function<void(msg_s*)> callback;
  int *counter;
} UDPThreadList;

struct msg_s{
  uint16_t seq_nr;
  uint16_t sender;
  uint16_t is_ack;
  uint16_t ack_from;
  //string msg;
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
  //void *thr_listener(void* arg);
  UDPThreadList threadListItem;
  //const char* ipaddr;
  std::vector<int> ports;
  int pid;
  int broadcast_count;
  int rec_from_1;
};
