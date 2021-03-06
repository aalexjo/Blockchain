#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <vector>

#include <assert.h>
#include <pthread.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "UDP.hpp"

#define BUFLEN 512

int rec_from_1;
int rec_from_3;
int rec_from_5;

void error(char const *e){
  perror(e);
  exit(1);
}

UDP::UDP(int pid, std::vector<int> ports, std::function<void(msg_s*)> callback): ports(ports){
  threadListItem = {ports[pid], (int)ports.size(), callback};
  broadcast_count = 0;
  this->pid = pid;
  this->n = ports.size();
}

//should be thread safe
void UDP::broadcast(struct msg_s* msg){

  struct sockaddr_in si_other;
  int s, slen=sizeof(si_other);

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) //setting up a UDP socket
    error("udp_send: socket");

  int broadcastEnable = 1;
  setsockopt(s,SOL_SOCKET,SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)); //enabling broadcast on this socket just in case

  memset((char *) &si_other, 0, sizeof(si_other)); //clearing si_other
  si_other.sin_family = AF_INET;

  uint8_t* buf = (uint8_t*)malloc(sizeof(int)*4 + sizeof(int)*this->n);
  memcpy(buf, msg, sizeof(int)*4);
  memcpy(buf + sizeof(int)*4, msg->VC, sizeof(int)*this->n);


  for(int i = 0; i < 1; i++){
    for( auto it = this->ports.begin(); it != this->ports.end(); it++){
      si_other.sin_port = htons(*it);//threadListItem.port);
      if (inet_aton("255.255.255.255", &si_other.sin_addr)==0) error("inet_aton() failed");//setting destination address
      int e = sendto(s, (void*) buf, sizeof(int)*4 + sizeof(int)*this->n, 0, (struct sockaddr *) &si_other, slen);//sending data
      if(e==-1)  error("udp_broadcast: sendto()");
    }
  }
  close(s);
  if(!(msg->is_ack))broadcast_count++;
  struct timespec sleep_time;
  sleep_time.tv_sec = 0;
  sleep_time.tv_nsec = 30;//these values are modifiable
  nanosleep(&sleep_time, NULL);//give the receiver time to catch up
}


void *thr_listener(void * arg){

  UDPThreadList * threadListItem = (UDPThreadList*) arg;

  struct sockaddr_in si_me, si_other;
  socklen_t slen = sizeof(si_other);

  int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  assert(s != -1);

  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(threadListItem->port);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  int optval = 1;
  setsockopt(s,SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval));

  int res = bind(s,(struct sockaddr *) &si_me, sizeof(si_me));
  if(res == -1) error("thr_udpListen:bind");

  msg_s* msg;
  uint8_t* buf = (uint8_t*)malloc(sizeof(int)*4 + sizeof(int)*threadListItem->n);
  while(1){
    msg = (msg_s*)malloc(sizeof(int)*4 + sizeof(int*));

    res = recvfrom(s, (void*)buf, (sizeof(int)*4 + sizeof(int)*threadListItem->n), 0,(struct sockaddr *) &si_other, &slen);
    if(res == -1) error("thr_udpListen:recvfrom");

    memcpy(msg, buf, sizeof(int)*4);
    msg->VC = (int*)malloc(sizeof(int)*threadListItem->n);
    memcpy(msg->VC, buf + sizeof(int)*4, sizeof(int)*threadListItem->n);

     ((threadListItem->callback))(msg);
  }

  // Never executed - this thread will be killed if it is not needed any more.
  close(s);
  return 0;
}

void UDP::startReceiving(){

  pthread_t listener;
  int e = pthread_create(&listener, NULL, thr_listener, &(this->threadListItem));
  if(e==-1)  error("udp_broadcast: pthread_create");
}

void UDP::udpPrint(){
  printf("PID: %d - num broadcasted messages %d \n", pid+1, broadcast_count);
  printf("PID: %d - received acks from 1 = %d\n", pid+1, rec_from_1);
  printf("PID: %d - received acks from 3 = %d\n", pid+1, rec_from_3);
  printf("PID: %d - received acks from 5 = %d\n", pid+1, rec_from_5);

}
