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
#include <mutex>
#include <cassert>
#include <iostream>
#include <vector>
#include <string>
#include <mutex>

using namespace std;

#define S2SP 1

static mutex muFwrd;

Client::Client(int pid, int process_n, int message_n, vector <int> id, vector <string> ips, vector <int> ports)
  : pid(pid), process_n(process_n),  message_n(message_n), id(id), ips(ips), ports(ports) {
  deliveredPL = vector<vector<vector<bool>>>(process_n, vector<vector<bool>>(message_n, vector<bool>(process_n, false)));
  deliveredURB = vector<vector<bool>>(process_n, vector<bool>(message_n, false));
  forwarded    = vector<vector<bool>>(process_n, vector<bool>(message_n, false));
  ack = vector<vector<vector<bool>>>(process_n, vector<vector<bool>>(message_n, vector<bool>(process_n, false)));
  curr_head = vector<int>(process_n, 0);
}

void Client::display(void) {
  cout << "Process index   : " << pid << '\n';
  cout << "Process total   : " << process_n << '\n';
  cout << "Messages count  : " << message_n << '\n';
  cout << "List of process : " << '\n';
  for (int i = 0; i < process_n; i++) {
    cout << id[i] << ":" << ips[i] << ":" << ports[i] << '\n';
  }
}

void Client::bebBroadcast(msg_s msg, int sockfd) {
  for(int dst = 0; dst < process_n; dst++) {
    // Trigger PP2PSend
    sendto_udp(msg, dst, sockfd);
  }
}

void Client::urbBroadcast(int seq_nbr, int sockfd) {
  msg_s msg = { false, pid, seq_nbr, pid};
  muFwrd.lock();
  forwarded[msg.creator][msg.seq_nbr] = true;
  for(int p = 0; p < process_n; p++)
    for(int m = 0; m < message_n; m++) {
      bool is_forwarded = forwarded[p][m];
      printf("[%i][%i]:%i\n", p, m, is_forwarded);
    }
  muFwrd.unlock();
  printf("SEND:m[%i,%i]:%i  | ack:%i\n", msg.creator, msg.seq_nbr, msg.src, msg.is_ack);

  // Trigger bebBroadcast
  bebBroadcast(msg, sockfd);
}

void Client::broadcastMessages(void) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd == -1){
    perror("cannot open socket");
    exit(1);
  }

  for(int seq_nbr = 0; seq_nbr < message_n; seq_nbr++) {
    // Trigger urbBroadcast
    urbBroadcast(seq_nbr, sockfd);
  }

  close(sockfd);
}

void Client::sendto_udp(msg_s msg, int dst, int sockfd) {
  sockaddr_in dest_addr;
  bzero(&dest_addr,sizeof(dest_addr));
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_addr.s_addr = inet_addr(ips[dst].c_str());
  dest_addr.sin_port = htons(ports[dst]);
  for(int i = 0; i < S2SP; i++)  {
    if (sendto(sockfd, (void* ) &msg, sizeof(msg), 0, (const sockaddr*) &dest_addr, sizeof(dest_addr)) == -1) {
      perror("cannot send message");
      exit(1);
    }
  }
}

void Client::startReceiving(void) {
  struct msg_s msg;
  sockaddr_in src_addr;
  socklen_t addrlen = sizeof(src_addr);

  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd == -1){
    perror("cannot open socket");
    exit(1);
  }

  bzero(&self_addr,sizeof(self_addr));
  self_addr.sin_family = AF_INET;
  self_addr.sin_addr.s_addr = inet_addr(ips[pid].c_str());
  self_addr.sin_port = htons(ports[pid]);
  if(bind(sockfd,(struct sockaddr *) &self_addr, sizeof(self_addr)) == -1) {
    perror("cannot bind socket");
    exit(1);
  }

  while (1) {
    if (recvfrom(sockfd, (void *) &msg, sizeof(msg), 0, (sockaddr*) &src_addr, (socklen_t *) &addrlen) == -1) {
      perror("cannot receive message");
      exit(1);
    }
    // Trigger flp2Deliver
    // Trigger sp2pDeliver
    // Event sp2pDeliver in pp2pDeliver
    if(!msg.is_ack) {
      msg_s new_msg;
      memcpy(&new_msg, &msg, sizeof(msg));
      new_msg.src = pid;


      if(!deliveredPL[msg.creator][msg.seq_nbr][msg.src]) {
        // Trigger pp2pDeliver
        // Event pp2pDeliver in bebDeliver
        // Trigger bebDeliver
        // Event bebDeliver in URB
        ack[msg.creator][msg.seq_nbr][msg.src] = true;

        muFwrd.lock();
        for(int p = 0; p < process_n; p++)
          for(int m = 0; m < message_n; m++) {
            bool is_forwarded = forwarded[p][m];
            printf("[%i][%i]:%i\n", p, m, is_forwarded);
          }
        if(!forwarded[msg.creator][msg.seq_nbr]) {
          forwarded[msg.creator][msg.seq_nbr] = true;
          muFwrd.unlock();
          printf("RESV:m[%i,%i]:%i  | ack:%i\n", msg.creator, msg.seq_nbr, msg.src, msg.is_ack);
          printf("SEND:m[%i,%i]:%i  | ack:%i\n", new_msg.creator, new_msg.seq_nbr, new_msg.src, new_msg.is_ack);

          bebBroadcast(new_msg, sockfd);


          // Trigger URB check for Delivery
          urbDeliverCheck(msg.creator, msg.seq_nbr);
        }
        // End bebDeliver trigger in URB
        // End bebDeliver trigger in beb
        deliveredPL[msg.creator][msg.seq_nbr][msg.src] = true;
        // End pp2pDeliver
      }

      // Ack
      new_msg.is_ack = true;
      if (sendto(sockfd, (void* ) &new_msg, sizeof(msg), 0, (const sockaddr*) &src_addr, addrlen) == -1) {
        perror("cannot send message");
        exit(1);
      }
    }
    // End sp2pDeliver
    // End flp2Deliver
  }
}

// Thread to be spawned that checks whenever process is done delivering everything necessary
void Client::urbDeliverCheck(int creator, int seq_nbr) {
  if (!deliveredURB[creator][seq_nbr]) {
    int nbr_rdy = 0;
    for(int p = 0; p < process_n; p++) {
      if(ack[creator][seq_nbr][p]) {
        nbr_rdy++;
      }
    }

    // Majoity Ack
    if(nbr_rdy > process_n/2) {
      //Trigger urbDeliver
      printf("URB:p(%i):%i. \n", creator, seq_nbr);
      deliveredURB[creator][seq_nbr] = true;
    }
  }

  while(deliveredURB[creator][curr_head[creator]]) {
    // Trigger FIFODeliver, pid, m=[creator, seq_nbr]
    curr_head[creator]++;
    printf("FIFO delivered");
  }
}
