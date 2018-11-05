#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>

#include "client.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void Client::display(void) {
  cout << "Process index   : " << process_i << '\n';
  cout << "Process total   : " << process_n << '\n';
  cout << "Messages count  : " << message_n << '\n';
  cout << "List of process : " << '\n';
  for (int i = 0; i < process_n; i++) {
    cout << id[i] << ":" << ips[i] << ":" << ports[i] << '\n';
  }
}

void Client::broadcast(void) {
  for(int p = 0; p < process_n; p++) {
    //if(p != process_i) {
    msg myMsg;
    myMsg.sender = process_i;
    myMsg.receiver = p;
    myMsg.msg_idx = msg_idx;
    sendto_udp(p, &myMsg);
    msg_idx++;
    //}
  }
}

int Client::sendto_udp(int process, const msg* msg_) {
  int res;
  sockaddr_in servaddr;
  int sockfd = socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd == -1){
    perror("cannot open socket");
    return sockfd;
  }

  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ips[process].c_str());
  servaddr.sin_port = htons(ports[process]);
  if ( (res = sendto(sockfd, (void* ) msg_, sizeof(msg_), 0, (const sockaddr*) &servaddr, sizeof(servaddr))) < 0) {
    perror("cannot send message");
  }
  close(sockfd);
  return res;
}

void Client::recv_msg_udp(void) {
  int res;
  msg myMsg;
  sockaddr_in servaddr;
  int sockFd = socket(AF_INET,SOCK_DGRAM,0);
  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ips[process_i].c_str());
  servaddr.sin_port = htons(ports[process_i]);

  if ( (res = recvfrom(sockfd, (void *) &myMsg, sizeof(myMsg), 0, (sockaddr*)&servaddr, sizeof(servaddr)) < 0) ) {
    perror("cannot receive message");
    exit(1);
  }
  printf("Received message");
  close(sockfd);
}
