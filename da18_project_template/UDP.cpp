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

void error(char const *e){
  perror(e);
  exit(1);
}

UDP::UDP(int pid, std::vector<int> ports, std::function<void(msg_s*)> callback): ports(ports){
  threadListItem = {ports[pid], callback};
  printf("ports: %d\n",ports[pid] );
}

void UDP::broadcast(struct msg_s* msg){//char const * data, int dataLength){
  struct sockaddr_in si_other;
  int s, slen=sizeof(si_other);

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) //setting up a UDP socket
    error("udp_send: socket");

  int broadcastEnable = 1;
  setsockopt(s,SOL_SOCKET,SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)); //enabling broadcast on this socket just in case

  memset((char *) &si_other, 0, sizeof(si_other)); //clearing si_other
  si_other.sin_family = AF_INET;

  //TODO: loop through all ports to send to
  //for( auto it = this->ports.begin(); it != this->ports.end(); ++it){
    //printf("Sent: msg-> sender %d\n", msg->sender);
    si_other.sin_port = htons(threadListItem.port);
    if (inet_aton("255.255.255.255", &si_other.sin_addr)==0) error("inet_aton() failed");//setting destination address
    int e = sendto(s, (void*) msg, sizeof(msg), 0, (struct sockaddr *) &si_other, slen);//sending data
    if(e==-1)  error("udp_broadcast: sendto()");
  //}
  close(s);//TODO: does closing the socket every time decrease performance? maybe open in an init function and save it in the class
}


void *thr_listener(void * arg){

  UDPThreadList * threadListItem = (UDPThreadList*) arg;

  struct sockaddr_in si_me, si_other;
  socklen_t slen = sizeof(si_other);
  //char buf[BUFLEN];

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
  //bzero(&msg, 2* sizeof(*msg));
  msg = (msg_s*)malloc(sizeof(msg_s));
  while(1){

    res = recvfrom(s, (void*)msg, sizeof(msg_s), 0,(struct sockaddr *) &si_other, &slen);
    if(res == -1) error("thr_udpListen:recvfrom");
    if(res >= BUFLEN-1){
      fprintf(stderr,"recvfrom: length of received message is larger than max message length: %d vs %d\n\n",res,BUFLEN);
      assert(res < BUFLEN-1);
    }
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
