#include "perfectLink.hpp"
#include <vector>

class reliableBroadcast{
public:
  reliableBroadcast(int m);

  void broadcast(char const * data, int dataLength);
  void receiver();
  bool canDeliver();
private:
  perfectLink link;
  int m;
  std::vector<bool> delivered;
  std::vector<bool> forward;
  std::vector<bool> correct;
  std::vector<std::vector<bool>> ack;


}
