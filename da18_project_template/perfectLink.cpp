//this module has lost most if its functionality as it is easier implemented at other layers
//we might want to migrate the rest to spawn the threads in URB and rebroadcast in UDP
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "perfectLink.hpp"

void *thr_broadcaster(void *arg) {

  perfectLinkThreadList * threadListItem = (perfectLinkThreadList*) arg;
  while(true){
    struct timespec sleep_time;
    sleep_time.tv_sec = 1;
    sleep_time.tv_nsec = 1;//these values are modifiable
    nanosleep(&sleep_time, NULL);//give the receiver time to catch up
    pthread_mutex_lock(threadListItem->list_lock);

    for(auto it = threadListItem->broadcast_list->rbegin();it != threadListItem->broadcast_list->rend(); it++){
      pthread_mutex_lock(threadListItem->broadcast_lock);
      threadListItem->udp->broadcast(it->second);
      pthread_mutex_unlock(threadListItem->broadcast_lock);
    }

    pthread_mutex_unlock(threadListItem->list_lock);

  }
  pthread_exit(0);
  return 0;
}


PerfectLink::PerfectLink(int pid, std::vector<int> ports, std::function<void(msg_s*)> callback){
  udp = new UDP(pid, ports, callback);
  pthread_mutex_init(&broadcast_lock, NULL);
  pthread_mutex_init(&list_lock, NULL);

  perfectLinkThreadList *threadListItem = new perfectLinkThreadList;//this must by dynamically allocated as threads live a long time
  threadListItem->udp = this->udp;
  threadListItem->broadcast_list = &broadcast_list;
  threadListItem->broadcast_lock = &broadcast_lock;
  threadListItem->list_lock = &list_lock;
  pthread_t broadcast;
  int e = pthread_create(&broadcast, NULL, thr_broadcaster, (threadListItem));
  if(e==-1) {
     error("perfectLink_broadcast: pthread_create");
  }
}


void PerfectLink::broadcast(struct msg_s* msg){
  pthread_mutex_lock(&broadcast_lock);
  udp->broadcast(msg);
  pthread_mutex_unlock(&broadcast_lock);


  if(msg->is_ack) return;

  pthread_mutex_lock(&list_lock);

  msg_s *new_msg = new msg_s;
  memcpy(new_msg, msg, sizeof(msg_s));

  broadcast_list.insert(broadcast_list.end(), std::pair<int, msg_s*>(msg->seq_nr, new_msg));
  pthread_mutex_unlock(&list_lock);


}

//this call might have to wait a long time for mutexes, try and make wait free?
void PerfectLink::stopBroadcast(int msg_seq_nr){
  pthread_mutex_lock(&list_lock);
  delete broadcast_list[msg_seq_nr];
  broadcast_list.erase(msg_seq_nr);
  pthread_mutex_unlock(&list_lock);

}

void PerfectLink::startReceiving(){
  this->udp->startReceiving();

}

//currently not in use
void PerfectLink::UDPcallback(struct msg_s* msg){
  //owner->pp2pCallback(this->owner, msg);
  printf("lolol\n" );
}

void PerfectLink::linkPrint(){
  //printf()'
  this->udp->udpPrint();
}
