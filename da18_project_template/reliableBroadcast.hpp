#include "perfectLink.hpp"
#include <vector>

class reliableBroadcast{
public:
  reliableBroadcast(int n, int pid);

  void broadcast(char const * data, int dataLength);
  void receiver();
  bool canDeliver();
private:
  perfectLink link;
  int n;
  int pid
  std::vector<bool> delivered;
  std::vector<bool> pending;
  std::vector<bool> correct;
  std::vector<int> ack;


}
