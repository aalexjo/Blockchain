#include "reliableBroadcast.hpp"
#include <cstdio>
#include <string>
typedef struct{
  reliableBroadcast *URB;
  int n;
  std::vector<int>* delivered;
  FILE * fout;
  std::vector<std::string>* output;
  pthread_mutex_t* output_lock;
}FIFOThreadList;

class FIFObroadcast{
public:
  FIFObroadcast(int n, int pid, std::vector<int> ports, int message_n);

  void broadcast(struct msg_s* msg);
  void startReceiving();
  void stopReceiving();
  void printOutput();
private:
  reliableBroadcast *URB;
  FIFOThreadList threadListItem;
  int n;
  int pid;
  int message_n;
  std::vector<int> delivered;
  FILE * fout;
  pthread_t broadcaster;//i am good at consistent naming
  std::vector<std::string> output;
  pthread_mutex_t output_lock;
};
