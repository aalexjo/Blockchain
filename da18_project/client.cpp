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
    struct req myReq;
    myReq.dst = p;
    sendto_req_udp(&myReq);
  }
  req_cnt++;
  printf("Fnished Broadcast. \n");
}

void Client::sendto_req_udp(struct req* req_) {
  int dst = req_->dst;
  req_->src = process_i;
  req_->req_cnt = req_cnt;

  struct sockaddr_in dest_addr;
  int sockfd = socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd == -1){
    perror("cannot open socket");
    exit(1);
  }

  bzero(&dest_addr,sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_addr.s_addr = inet_addr(ips[dst].c_str());
  dest_addr.sin_port = htons(ports[dst]);

  if (sendto(sockfd, (void* ) req_, sizeof(req_), 0, (const struct sockaddr*) &dest_addr, sizeof(dest_addr)) == -1) {
    perror("cannot send message");
    exit(1);
  }
  close(sockfd);
  cout << "TX:" << req_->src << "->" << req_->dst << ":" << req_->req_cnt << '\n';
  cout << " ->" << dest_addr.sin_addr.s_addr << ":" << dest_addr.sin_port << '\n';
}

void Client::recvfrom_req_udp(struct req* req_) {
  struct sockaddr_in src_addr;
  bzero(&src_addr,sizeof(src_addr));
  src_addr.sin_family = AF_INET;
  src_addr.sin_addr.s_addr = inet_addr(ips[process_i].c_str());
  src_addr.sin_port = htons(ports[process_i]);

  int sockfd = socket(AF_INET,SOCK_DGRAM,0);
  if(sockfd == -1){
    perror("cannot open socket");
    exit(1);
  }

  if(bind(sockfd, (struct sockaddr *) &src_addr, sizeof(src_addr))) {
    perror("cannot bind socket");
    exit(1);
  }

  if (recvfrom(sockfd, (void *) req_, sizeof(req_), 0, (sockaddr*) &src_addr, 0) == -1) {
    perror("cannot receive message");
    exit(1);
  }
  close(sockfd);
  cout << "RX:" << req_->src << "->" << req_->dst << ":" << req_->req_cnt << '\n';
}
