#include "reliableBroadcast.hpp"

class FIFObroadcast{
public:
  FIFObroadcast(int n, int pid, std::vector<int> ports);

  void broadcast(struct msg_s* msg);
  void startReceiving();

private:
  reliableBroadcast URB;
  int n;
  int pid;
  std::vector<int> delivered;

};
