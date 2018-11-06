#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "perfectLink.hpp"

PerfectLink::PerfectLink(int pid, std::vector<int> ports, std::function<void(msg_s*)> callback): udp(pid, ports, callback){

}

void *thr_broadcaster(void *arg) {

  perfectLinkThreadList * threadListItem = (perfectLinkThreadList*) arg;

  for (int i = 0; i < 1; i++) {
  	threadListItem->udp.broadcast(threadListItem -> msg);
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 50;
    nanosleep(&sleep_time, NULL);
  }
  return 0;
}

void PerfectLink::broadcast(struct msg_s* msg){

  static perfectLinkThreadList threadListItem = {this->udp, msg};
  pthread_t broadcaster;
  int e = pthread_create(&broadcaster, NULL, thr_broadcaster, &(threadListItem));
  if(e==-1) {
     error("perfectLink_broadcast: pthread_create");
  }
}

void PerfectLink::startReceiving(){
  this->udp.startReceiving();

}

void PerfectLink::UDPcallback(struct msg_s* msg){
  //owner->pp2pCallback(this->owner, msg);
  printf("lolol\n" );
}
