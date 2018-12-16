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

using namespace std;

struct msg_s {
  int creator;
  int src;
  int seq_nbr;
  int* VC;
  bool is_ack;
};


struct process_s {
  int id;
  string ip;
  int port;
};

class UDP {
private :
  int pid;
  int processNbr;
  int messageNbr;
  vector <process_s> processes;
  function<void(msg_s*)> triggerCallbackUDP;

public :
  UDP(int pid, int processNbr, int messageNbr, vector <process_s> processes, function<void(msg_s*)> callback) :
    pid(pid), processNbr(processNbr), messageNbr(messageNbr), processes(processes), triggerCallbackUDP(callback) {

  };

  void send(msg_s msg, int dst, int sockfd) {
    sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = inet_addr(processes[dst].ip.c_str());
    dest_addr.sin_port = htons(processes[dst].port);
    printf("S:pid:%i:msg:[%i:m[%i,%i]]\n", pid, msg.creator, msg.src, msg.seq_nbr);
    if (sendto(sockfd, (void* ) &msg, sizeof(msg_s), 0, (const sockaddr*) &dest_addr, sizeof(dest_addr)) == -1) {
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
    while(1) {
      if (recvfrom(sockfd, (void *) &msg, sizeof(msg_s), 0, (sockaddr*) &src_addr, (socklen_t *) &addrlen) == -1) {
        perror("cannot receive message");
        exit(1);
      }
      printf("R:pid:%i:msg:[%i:m[%i,%i]]\n", pid, msg.creator, msg.src, msg.seq_nbr);
      triggerCallbackUDP(&msg);
    }
  }
};
