#include "reliableBroadcast.hpp"

class FIFObroadcast{
public:
  FIFObroadcast();

  void broadcast(struct msg_s* msg);
  void startReceiving();

private:
  reliableBroadcast URB;
  int n;
  int pid;
  std::vector<int> delivered;

}
