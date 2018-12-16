// Main
#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
// Socket
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
// Structs
#include <vector>
#include <string>
#include <functional>
#include <thread>

using namespace std;

struct msg_s {
  int src;
  int creator;
  int seq_nbr;
  bool is_ack;
  int* VC;
};

struct process_s {
  int id;
  string ip;
  int port;
};

class UDP {
protected:
  int pid;
  int processNbr;
  int messageNbr;
  vector <process_s> processes;
  vector< vector< vector<bool> > > ack;
private:
  size_t msgSize;
  size_t VCSize;
  function<void(msg_s)> triggerCallbackUDP;

public :
  UDP(int pid, int processNbr, int messageNbr, vector <process_s> processes, function<void(msg_s)> callback) :
    pid(pid), processNbr(processNbr), messageNbr(messageNbr), processes(processes), triggerCallbackUDP(callback) {
    ack = vector<vector<vector<bool>>>(processNbr, vector<vector<bool>>(messageNbr, vector<bool>(processNbr, false)));
    VCSize = sizeof(int) * processNbr;
    msgSize = sizeof(msg_s) + VCSize;
  };

  void send(msg_s msg, int dst, int sockfd) {
    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(processes[dst].ip.c_str());
    dest_addr.sin_port = htons(processes[dst].port);
    printf("S:pid:%i:msg:[%i:m[%i,%i]]\n", pid, msg.creator, msg.src, msg.seq_nbr);
    size_t* buf = (size_t*)malloc(msgSize);
    memcpy(buf, &msg, sizeof(msg_s));
    memcpy(buf + sizeof(msg_s), msg.VC, VCSize);
    if (sendto(sockfd, (void* ) &msg, msgSize, 0, (const sockaddr*) &dest_addr, sizeof(dest_addr)) == -1) {
      perror("cannot send message");
      exit(1);
    }
  }

  void startReceiving(void) {
    msg_s msg;
    sockaddr_in src_addr;
    sockaddr_in self_addr;
    socklen_t addrlen = sizeof(src_addr);

    int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(sockfd == -1){
      perror("cannot open socket");
      exit(1);
    }

    memset(&self_addr, 0, sizeof(self_addr));
    self_addr.sin_family = AF_INET;
    self_addr.sin_addr.s_addr = inet_addr(processes[pid].ip.c_str());
    self_addr.sin_port = htons(processes[pid].port);

    if(bind(sockfd,(struct sockaddr *) &self_addr, sizeof(self_addr)) == -1) {
      perror("cannot bind socket");
      exit(1);
    }
    size_t* buf = (size_t*)malloc(msgSize);
    while(1) {
      if (recvfrom(sockfd, (void *) buf, msgSize, 0, (sockaddr*) &src_addr, (socklen_t *) &addrlen) == -1) {
        perror("cannot receive message");
        exit(1);
      }
      msg.VC = (int*) malloc(VCSize);
      memcpy(&msg, buf, sizeof(msg_s));
      memcpy(msg.VC, buf + sizeof(msg_s), VCSize);

      if(!msg.is_ack) {
        printf("R:pid:%i:msg:[%i:m[%i,%i]]\n", pid, msg.creator, msg.src, msg.seq_nbr);
        thread t(triggerCallbackUDP, msg);
        t.detach();

        msg.is_ack = true;
        memcpy(buf, &msg, sizeof(msg_s));
        memcpy(buf + sizeof(msg_s), msg.VC, VCSize);
        if (sendto(sockfd, (void* ) buf, msgSize, 0, (const sockaddr*) &src_addr, addrlen) == -1) {
          perror("cannot send message");
          exit(1);
        }
        printf("Sended Ack\n");
      } if (!ack[msg.src][msg.creator][msg.seq_nbr]){
        printf("ACK:pid:%i:msg:[%i:m[%i,%i]]\n", pid, msg.src, msg.creator, msg.seq_nbr);
        ack[msg.src][msg.creator][msg.seq_nbr] = true;
      }
    }
  }
};
