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
#include <thread>

using namespace std;

#define S2SP 1

Client::Client(int pid, int process_n, int message_n, vector <int> id, vector <string> ips, vector <int> ports)
  : pid(pid), process_n(process_n),  message_n(message_n), id(id), ips(ips), ports(ports) {
  deliveredPL = vector<vector<vector<bool>>>(process_n, vector<vector<bool>>(message_n, vector<bool>(process_n, false)));
  deliveredURB = vector<vector<bool>>(process_n, vector<bool>(message_n, false));
  forwarded    = vector<vector<bool>>(process_n, vector<bool>(message_n, false));
  ackPL = vector<vector<vector<bool>>>(process_n, vector<vector<bool>>(message_n, vector<bool>(process_n, false)));
  ackURB = vector<vector<vector<bool>>>(process_n, vector<vector<bool>>(message_n, vector<bool>(process_n, false)));
  curr_head = vector<int>(process_n, 0);
  string fname = "da_proc_" + to_string(pid) + ".txt";
  fout = fopen(fname.c_str(), "w+");

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

void Client::bebBroadcast(msg_s msg) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if(sockfd == -1){
    perror("cannot open socket");
    exit(1);
  }

  for(int dst = 0; dst < process_n; dst++) {
    // Trigger PP2PSend
    sendto_udp(msg, dst, sockfd);
  }

  close(sockfd);
}

void Client::urbBroadcast(int seq_nbr) {
  msg_s msg = { false, pid, seq_nbr, pid};
  forwarded[msg.creator][msg.seq_nbr] = true;
  //printf("BROADCAST:SEND:[%i,m[%i,%i]]\n", msg.src, msg.creator, msg.seq_nbr);
  fprintf(fout, "b %d\n", msg.seq_nbr);

  // Trigger bebBroadcast
  bebBroadcast(msg);
}

void Client::broadcastMessages(void) {
  for(int seq_nbr = 0; seq_nbr < message_n; seq_nbr++) {
    // Trigger urbBroadcast
    urbBroadcast(seq_nbr);
  }

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
		struct timespec timeout;
		timeout.tv_sec = 0;
		timeout.tv_nsec = 200;
		nanosleep(&timeout, NULL);
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

    if(!msg.is_ack) {
      // Trigger flp2Deliver
      // Trigger sp2pDeliver
      // Event sp2pDeliver in pp2pDeliver
      if(!deliveredPL[msg.creator][msg.seq_nbr][msg.src]) {
        //fprintf(fout, "pid:%i:PL  :DELV:[%i:m[%i,%i]]\n", pid, msg.src, msg.creator, msg.seq_nbr);
        // Trigger pp2pDeliver
        //printf("pid:%i:PL  :DELV:[%i:m[%i,%i]]\n", pid, msg.src, msg.creator, msg.seq_nbr);
        deliveredPL[msg.creator][msg.seq_nbr][msg.src] = true;
        // Event pp2pDeliver in bebDeliver
        // Trigger bebDeliver
        // Event bebDeliver in URB
        ackURB[msg.creator][msg.seq_nbr][msg.src] = true;

        if(!forwarded[msg.creator][msg.seq_nbr]) {
          forwarded[msg.creator][msg.seq_nbr] = true;
          //printf("pid:%i:FORWARD:SEND:[%i,m[%i,%i]]\n", pid, new_msg.src, new_msg.creator, new_msg.seq_nbr);
          msg_s new_msg;
          memcpy(&new_msg, &msg, sizeof(msg));
          new_msg.src = pid;
          thread t(&Client::bebBroadcast, this, new_msg);
          t.detach();
        }
        urbDeliverCheck(msg.creator, msg.seq_nbr);
        // End bebDeliver trigger in URB
        // End pp2pDeliver
      }
      // End sp2pDeliver
      // End flp2Deliver

      msg.is_ack = true;
      if (sendto(sockfd, (void* ) &msg, sizeof(msg), 0, (const sockaddr*) &src_addr, addrlen) == -1) {
        perror("cannot send message");
        exit(1);
      }
    } else if (msg.is_ack && !ackPL[msg.creator][msg.seq_nbr][msg.src]) { // Second for less priting
      //fprintf(fout, "pid:%i:PL:ACK:[%i,m[%i,%i]]\n", pid, msg.src, msg.creator, msg.seq_nbr);
      ackPL[msg.creator][msg.seq_nbr][msg.src] = true;
    }
  }
}

// Thread to be spawned that checks whenever process is done delivering everything necessary
void Client::urbDeliverCheck(int creator, int seq_nbr) {
  int nbr_rdy = 0;
  if (!deliveredURB[creator][seq_nbr]) {
    for(int p = 0; p < process_n; p++) {
      if(ackURB[creator][seq_nbr][p]) {
        nbr_rdy++;
      }
    }
    //printf("NBR_RDY: %i\n", nbr_rdy);

    // Majoity Ack
    if(nbr_rdy > (process_n-1)/2) {
      //Trigger urbDeliver
      //printf("pid:%i:URB :DELV:m[%i,%i]. \n", pid, creator, seq_nbr);
      deliveredURB[creator][seq_nbr] = true;
    }
  }

  while(deliveredURB[creator][curr_head[creator]] && curr_head[creator] != message_n) {
    // Trigger FIFODeliver
    //printf("pid:%i:FIFO:DELV:m[%i,%i]\n", pid, creator, curr_head[creator]);
    int m_nbr = curr_head[creator];
    fprintf(fout, "d %d %d\n", creator, m_nbr);
    curr_head[creator]++;
  }
}
