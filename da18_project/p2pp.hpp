#include <sys/socket.h>
#include <netinet/in.h>

#include <vector>
#include <string>
#include <functional>

#include "udp.hpp"

using namespace std;

class P2PP : public UDP {
private:
  function<void(msg_s*)> triggerCallbackP2PP;
protected:
  vector< vector< vector<bool> > > deliveredPL, ackPL;

public :
  P2PP(int pid, int processNbr, int messageNbr, vector<process_s> processes, function<void(msg_s*)> callback) :
    UDP(pid, processNbr, messageNbr, processes, bind(&P2PP::pp2pReceive, this, placeholders::_1)), triggerCallbackP2PP(callback) {
    deliveredPL = vector<vector<vector<bool>>>(processNbr, vector<vector<bool>>(messageNbr, vector<bool>(processNbr, false)));
    ackPL = vector<vector<vector<bool>>>(processNbr, vector<vector<bool>>(messageNbr, vector<bool>(processNbr, false)));
  };

  void send(msg_s msg, int dst, int sockfd) {
    UDP::send(msg, dst, sockfd);
  }

  void pp2pReceive(msg_s* msg) {
    printf("Received\n");
    if(!deliveredPL[msg->creator][msg->seq_nbr][msg->src]) {
      printf("DL:P2PP:pid:%i:msg:[%i:m[%i,%i]]\n", UDP::pid, msg->creator, msg->src, msg->seq_nbr);
      deliveredPL[msg->creator][msg->seq_nbr][msg->src] = true;
      triggerCallbackP2PP(msg);
    }
  }

};

