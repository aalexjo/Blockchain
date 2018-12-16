#include <sys/socket.h>
#include <netinet/in.h>

#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <utility>
#include <list>
#include <iterator>

#include "udp.hpp"

using namespace std;

struct msg_stack_s {
  msg_s msg;
  int dst;
};

class P2PP : public UDP {
private:
  function<void(msg_s*)> triggerCallbackP2PP;
  void pp2pReceive(msg_s msg) {
    if(!deliveredPL[msg.creator][msg.seq_nbr][msg.src]) {
      printf("DL:P2PP:pid:%i:msg:[%i:m[%i,%i]]\n", UDP::pid, msg.creator, msg.src, msg.seq_nbr);
      deliveredPL[msg.creator][msg.seq_nbr][msg.src] = true;
      triggerCallbackP2PP(&msg);
    }
  }

protected:
  vector< vector< vector<bool> > > deliveredPL;
  pthread_mutex_t msg_stack_add_lock;
  list<msg_stack_s> msg_send_stack;
  list<msg_stack_s> msg_send_add_stack;

public :
  P2PP(int pid, int processNbr, int messageNbr, vector<process_s> processes, function<void(msg_s*)> callback) :
    UDP(pid, processNbr, messageNbr, processes, bind(&P2PP::pp2pReceive, this, placeholders::_1)), triggerCallbackP2PP(callback) {
    deliveredPL = vector<vector<vector<bool>>>(processNbr, vector<vector<bool>>(messageNbr, vector<bool>(processNbr, false)));
    pthread_mutex_init(&msg_stack_add_lock, NULL);
  };

  void send(msg_s msg, int dst) {
    pthread_mutex_lock(&msg_stack_add_lock);
    msg_stack_s msg_stack = {msg, dst};
    msg_send_add_stack.push_back(msg_stack);
    pthread_mutex_unlock(&msg_stack_add_lock);

    deliveredPL[msg.creator][msg.seq_nbr][msg.src] = true;
  }

  void pp2pStartSend(void) {
    struct timespec sleep_time;
    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd == -1){
      perror("cannot open socket");
      exit(1);
    }
    while(1) {
      // Send messages
      std::list<msg_stack_s>::iterator i = msg_send_stack.begin();
      while (i != msg_send_stack.end()) {
        msg_s msg = (*i).msg;
        int dst = (*i).dst;
        if (ack[msg.src][msg.creator][msg.seq_nbr]) {
          msg_send_stack.erase(i++);
        } else {
          UDP::send(msg, dst, sockfd);
          ++i;
        }
      }

      // Add new messages
      pthread_mutex_lock(&msg_stack_add_lock);
      msg_send_stack.splice(msg_send_stack.end(), msg_send_add_stack);
      pthread_mutex_unlock(&msg_stack_add_lock);

      // Wait for next iteration
      sleep_time.tv_sec = 0;
      sleep_time.tv_nsec = 1000;
      nanosleep(&sleep_time, NULL);
    }
  }
};
