#include <sys/socket.h>
#include <netinet/in.h>

#include <vector>
#include <string>
#include <functional>

#include "udp.hpp"

using namespace std;

class P2PP : public UDP {
private :
  vector< vector< vector<bool> > > deliveredPL, ackPL;
  function<void(msg_s*)> triggerCallbackP2PP;

public :
  void send(msg_s msg, int dst, int sockfd) {
    UDP::send(msg, dst, sockfd);
  }

  void pp2pReceive(msg_s* msg) {
    if(!deliveredPL[msg->creator][msg->seq_nbr][msg->src]) {
      deliveredPL[msg->creator][msg->seq_nbr][msg->src] = true;
      triggerCallbackP2PP(msg);
    }
  }

  P2PP(int pid, int processNbr, int messageNbr, vector<process_s> processes, function<void(msg_s*)> callback) :
    UDP(pid, processNbr, messageNbr, processes, bind(&P2PP::pp2pReceive, this, placeholders::_1)), triggerCallbackP2PP(callback) {};

};

