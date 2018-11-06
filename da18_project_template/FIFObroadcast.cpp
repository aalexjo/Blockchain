#include "FIFObroadcast.hpp"
#include <iostream>

FIFObroadcast::FIFObroadcast(int n, int pid, std::vector<int> ports): n(n), pid(pid), delivered(n, 0){
  URB = new reliableBroadcast(n, pid, ports);
}

void FIFObroadcast::broadcast(struct msg_s* msg){
  URB->broadcast(msg);
  std::cout<<"b "<<msg->seq_nr<<std::endl;
}


void *thr_receiver(void *arg) {

  FIFOThreadList * threadListItem = (FIFOThreadList*) arg;

  while(true){
    for(int i = 0; i<=threadListItem->n; i++){
      if (threadListItem->URB->canDeliver(i, (*threadListItem->delivered)[i])){
        //TODO:deliver that shit
        std::cout<<"d "<< i << ++(*threadListItem->delivered)[i] << std::endl;
      }
    }
  }
}

void FIFObroadcast::startReceiving(){

  static FIFOThreadList threadListItem = {this->URB,n, &delivered};
  pthread_t broadcaster;
  int e = pthread_create(&broadcaster, NULL, thr_receiver, &(threadListItem));
  if(e==-1) {
     error("perfectLink_broadcast: pthread_create");
  }
}
