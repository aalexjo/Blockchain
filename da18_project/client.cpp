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

Client::Client(int process_i, int process_n, int message_n, vector <int> id, vector <string> ips, vector <int> ports)
  : process_i(process_i), process_n(process_n),  message_n(message_n), id(id), ips(ips), ports(ports) {
  delivered = vector<vector<int>>(process_n, vector<int>(0));
}

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
  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd == -1){
    perror("cannot open socket");
    exit(1);
  }
  for(int p = 0; p < process_n; p++) {
    sendto_req_udp(p, sockfd, BEB);
  }
  seq_nbr += 1;
}

void Client::sendto_req_udp(int dst, int sockfd, Layer layer) {
  struct req req_;
  req_.dst = dst;
  req_.src = process_i;
  req_.seq_nbr = seq_nbr;
  req_.layer = layer;

  sockaddr_in dest_addr;
  bzero(&dest_addr,sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_addr.s_addr = inet_addr(ips[dst].c_str());
  dest_addr.sin_port = htons(ports[dst]);

  //printf("TX:%u:%u->%u:%i\n", req_.layer, req_.src, req_.dst, req_.seq_nbr);
  cout << "TX:" << req_.layer << ":" << req_.src << "->" << req_.dst << ":" << req_.seq_nbr << '\n';
  if (sendto(sockfd, (void* ) &req_, sizeof(req_), 0, (const sockaddr*) &dest_addr, sizeof(dest_addr)) == -1) {
    perror("cannot send message");
    exit(1);
  }
}

void Client::startReceiving(void) {
  struct req req_;
  sockaddr_in src_addr;
  socklen_t addrlen;

  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd == -1){
    perror("cannot open socket");
    exit(1);
  }

  bzero(&self_addr,sizeof(self_addr));
  self_addr.sin_family = AF_INET;
  self_addr.sin_addr.s_addr = inet_addr(ips[process_i].c_str());
  self_addr.sin_port = htons(ports[process_i]);
  if(bind(sockfd,(struct sockaddr *) &self_addr, sizeof(self_addr)) == -1) {
    perror("cannot bind socket");
    exit(1);
  }

  while (1) {
    if (recvfrom(sockfd, (void *) &req_, sizeof(req_), 0, (sockaddr*) &src_addr, &addrlen) == -1) {
      perror("cannot receive message");
      exit(1);
    }
    cout << "RX:" << req_.layer << ":" << req_.src << "->" << req_.dst << ":" << req_.seq_nbr << '\n';
  }
}
