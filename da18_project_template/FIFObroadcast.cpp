#include "FIFObroadcast.hpp"
#include <iostream>
#include <string>
FIFObroadcast::FIFObroadcast(int n, int pid, std::vector<int> ports): n(n), pid(pid), delivered(n, 0){
  URB = new reliableBroadcast(n, pid, ports);
  std::string fname = "da_proc_" + std::to_string(pid+1) + ".txt";
  fout = fopen(fname.c_str(), "w+");
}

void FIFObroadcast::broadcast(struct msg_s* msg){
  fprintf(fout, "b %d\n", msg->seq_nr);
  //std::cout<<"b "<<msg->seq_nr<<std::endl;
  URB->broadcast(msg);
}


void *thr_receiver(void *arg) {

  FIFOThreadList * threadListItem = (FIFOThreadList*) arg;

  while(true){
    for(int i = 0; i<threadListItem->n; i++){
      if (threadListItem->URB->canDeliver(i, (*threadListItem->delivered)[i]+1)){
        //TODO:deliver that shit
        fprintf(threadListItem->fout, "d %d %d\n", i+1, ++(*threadListItem->delivered)[i] );
        //std::cout<<"d "<< i+1 << " " << ++(*threadListItem->delivered)[i] << std::endl;
      }
    }
  }
}

void FIFObroadcast::startReceiving(){

  static FIFOThreadList threadListItem = {this->URB,n, &delivered, this->fout};
  pthread_t broadcaster;
  int e = pthread_create(&broadcaster, NULL, thr_receiver, &(threadListItem));
  if(e==-1) {
     error("perfectLink_broadcast: pthread_create");
  }
}
