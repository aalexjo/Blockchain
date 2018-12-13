#include "FIFObroadcast.hpp"
#include <vector>

typedef struct{
  reliableBroadcast *URB;
  int n;
  std::vector<int>* delivered;
  std::vector<std::string>* output;
  pthread_mutex_t* output_lock;
}CAThreadList;


class causalBroadcast{
public:
  causalBroadcast(int n, int pid, std::vector<int> ports, int message_n, std::vector<int> dependencies);

  void broadcast(struct msg_s* msg);
  void startReceiving();
  void stopReceiving();
  void printOutput();

private:
  reliableBroadcast *URB;
  std::vector<int> dependencies;
  int n;
  int pid;
  int message_n;
  int** VC;
  std::vector<msg_s> pending;
  pthread_t reveiver;

  FILE * fout;
  std::vector<std::string> output;
  pthread_mutex_t output_lock;

};
