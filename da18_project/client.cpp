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
  for(int i = 0; i < process_n; i++) {
    if(i != process_i) {
      sendto_udp(i);
    }
  }
}

int Client::sendto_udp(int process) {
  char msg[3] = "ms";
  sockaddr_in servaddr;
  int fd = socket(AF_INET,SOCK_DGRAM,0);
  if(fd == -1){
    perror("cannot open socket");
    return fd;
  }

  bzero(&servaddr,sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ips[process].c_str());
  servaddr.sin_port = htons(ports[process]);
  if (( fd = sendto(fd, msg, strlen(msg)+1, 0, // +1 to include terminator
                    (sockaddr*)&servaddr, sizeof(servaddr)) < 0)) {
    perror("cannot send message");
  }
  close(fd);
  return fd;
}
