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
  done = vector<bool>(process_n, false);
  ackDone = vector<bool>(process_n, false);
  string fname = "da_proc_" + to_string(pid+1) + ".out";
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
    //if(ackDone[dst] && !done[dst])
      sendto_udp(msg, dst, sockfd);
  }
  close(sockfd);
}

void Client::urbBroadcast(int seq_nbr) {
  msg_s msg = { isMsg, pid, seq_nbr, pid, false};
  forwarded[msg.creator][msg.seq_nbr] = true;
  //printf("BROADCAST:SEND:[%i,m[%i,%i]]\n", msg.src, msg.creator, msg.seq_nbr);
  fprintf(fout, "b %d\n", msg.seq_nbr+1);

  // Trigger bebBroadcast
  bebBroadcast(msg);
}

void Client::broadcastMessages(void) {
  /*
  int done_cnt = 0;
  struct timespec timeout;
  while(done_cnt != process_n) {
    done_cnt = 0;
    msg_s msg = { isDoneReq, pid, seq_nbr, pid, false};
    bebBroadcast(msg);

		timeout.tv_sec = 0;
		timeout.tv_nsec = 100;
		nanosleep(&timeout, NULL);
    for(int p = 0; p < process_n; p++) {
      if(ackDone[p]) printf("done:%i", p);
    }
    // */
    for(int seq_nbr = 0; seq_nbr < message_n; seq_nbr++) {
      // Trigger urbBroadcast
      urbBroadcast(seq_nbr);
    }

	  /*
    timeout.tv_sec = 2;
		timeout.tv_nsec = 0;
		nanosleep(&timeout, NULL);
    for(int p = 0; p < process_n; p++) {
      ackDone[p] = false;
      if(done[p]) done_cnt++;
      if(done[p]) printf("done:%i", p);
    }
  }
  // */
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

    /*
    switch(msg.type) {
    case isMsg: printf("msg.type:isMsg\n");
    case isMsgAck: printf("msg.type:isMsgAck\n");
    case isDoneReq: printf("msg.type:isDoneReq\n");
    case isDoneAck: printf("msg.type:isDoneAck\n");
    }
    // */

    switch(msg.type) {
    case isMsg:
      // Trigger sp2pDeliver
      if(!deliveredPL[msg.creator][msg.seq_nbr][msg.src]) {
        // Trigger pp2pDeliver
        deliveredPL[msg.creator][msg.seq_nbr][msg.src] = true;
        // Trigger bebDeliver
        ackURB[msg.creator][msg.seq_nbr][msg.src] = true;
        if(!forwarded[msg.creator][msg.seq_nbr]) {
          forwarded[msg.creator][msg.seq_nbr] = true;
          //printf("pid:%i:FORWARD:SEND:[%i,m[%i,%i]]\n", pid, new_msg.src, new_msg.creator, new_msg.seq_nbr);
          msg_s new_msg = { isMsg, msg.creator, msg.seq_nbr, pid, false };
          thread t(&Client::bebBroadcast, this, new_msg);
          t.detach();
        }
        urbDeliverCheck(msg.creator, msg.seq_nbr);
        if (curr_head[msg.creator] == message_n) {
          msg_s new_msg;
          new_msg = { isDoneAck, msg.creator, 0, pid, true };
          sendto_udp(new_msg, msg.creator, sockfd);
        }
        // End bebDeliver Trigger
        // End pp2pDeliver Trigger
      }
      // End sp2pDeliver Trigger

      // Ack
      msg.type = isMsgAck;
      if (sendto(sockfd, (void* ) &msg, sizeof(msg), 0, (const sockaddr*) &src_addr, addrlen) == -1) {
        perror("cannot send message");
        exit(1);
      }
      break;
    case isMsgAck:
      ackPL[msg.creator][msg.seq_nbr][msg.src] = true;
      break;
    case isDoneReq:
      msg_s new_msg;
      new_msg = { isDoneAck, msg.creator, 0, pid, (curr_head[msg.creator] == message_n)};
      if (sendto(sockfd, (void* ) &new_msg, sizeof(new_msg), 0, (const sockaddr*) &src_addr, addrlen) == -1) {
        perror("cannot send message");
        exit(1);
      }
      break;
    case isDoneAck:
      ackDone[msg.src] = true;
      done[msg.src] = msg.done;
      break;
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
    if(nbr_rdy > (process_n)/2) {
      //Trigger urbDeliver
      //printf("pid:%i:URB :DELV:m[%i,%i]. \n", pid, creator, seq_nbr);
      deliveredURB[creator][seq_nbr] = true;
    }
  }

  while(deliveredURB[creator][curr_head[creator]] && curr_head[creator] != message_n) {
    // Trigger FIFODeliver
    //printf("pid:%i:FIFO:DELV:m[%i,%i]\n", pid, creator, curr_head[creator]);
    int m_nbr = curr_head[creator];
    fprintf(fout, "d %d %d\n", creator+1, m_nbr+1);
    curr_head[creator]++;
  }
}
