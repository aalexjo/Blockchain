//this module has lost most if its functionality as it is easier implemented at other layers
//we might want to migrate the rest to spawn the threads in URB and rebroadcast in UDP
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "perfectLink.hpp"

PerfectLink::PerfectLink(int pid, std::vector<int> ports, std::function<void(msg_s*)> callback){
  udp = new UDP(pid, ports, callback);
}

void *thr_broadcaster(void *arg) {

  perfectLinkThreadList * threadListItem = (perfectLinkThreadList*) arg;

  //ideally this should not be a for loop but
  //while(received[threadListItem->msg->seq_nr]<=n/2)
  //where received[i] is the number of other processes that we have received an ack from on a given seq_nr
  for (int i = 0; i < 1; i++) {
  	threadListItem->udp->broadcast(threadListItem->msg);
    // struct timespec sleep_time;
    // sleep_time.tv_sec = 0;
    // sleep_time.tv_nsec = 0;//these values are modifiable
    //nanosleep(&sleep_time, NULL);
  }

  delete threadListItem->msg;
  delete threadListItem;
  //TODO: free memory allocated to thread list and msg!!
  return 0;
}

void PerfectLink::broadcast(struct msg_s* msg){
  udp->broadcast(msg);
//using pthread now because it does not crash when many threads are spawned
//maybe it does not let us now that we spawn to many threads? might need to terminate
//a thread if we know that it has received acks from all other ps.
//can also check if we are sending an ack and not spawn a new thread for those

  // if(msg->is_ack){
  //   //this->udp->broadcast(msg);
  //   return;
  // }
  // perfectLinkThreadList *threadListItem = new perfectLinkThreadList;//this must by dynamically allocated as threads live a long time
  // threadListItem->udp = this->udp;
  // msg_s *new_msg = new msg_s;
  // memcpy(new_msg, msg, sizeof(*msg));
  // threadListItem->msg = new_msg;
  //
  // pthread_t broadcaster;
  // int e = pthread_create(&broadcaster, NULL, thr_broadcaster, (threadListItem));
  // if(e==-1) {
  //    error("perfectLink_broadcast: pthread_create");
  // }
  // e = pthread_join(broadcaster, NULL);
  // if(e==-1) {
  //    error("perfectLink_broadcast: pthread_join");
  // }
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
