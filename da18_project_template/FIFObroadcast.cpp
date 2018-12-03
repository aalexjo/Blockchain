#define _GLIBCXX_USE_CXX11_ABI 0

#include "FIFObroadcast.hpp"
#include <iostream>
#include <string>
FIFObroadcast::FIFObroadcast(int n, int pid, std::vector<int> ports): n(n), pid(pid), delivered(n, 0){
  URB = new reliableBroadcast(n, pid, ports);
  std::string fname = "da_proc_" + std::to_string(pid+1) + ".out";
  fout = fopen(fname.c_str(), "w+");
  pthread_mutex_init(&(this->output_lock),NULL);

  threadListItem.URB = URB;
  threadListItem.n = n;
  threadListItem.delivered = &delivered;
  threadListItem.fout = fout;
  threadListItem.output = &output;
  threadListItem.output_lock = &output_lock;
}

void FIFObroadcast::broadcast(struct msg_s* msg){
  pthread_mutex_lock(&(this->output_lock));
  output.push_back("b " +  std::to_string(msg->seq_nr));
  pthread_mutex_unlock(&(this->output_lock));
  URB->broadcast(msg);
}


void *thr_receiver(void *arg) {
  FIFOThreadList * threadListItem = (FIFOThreadList*) arg;

  while(true){ //should we ever terminate?
    for(int i = 0; i<threadListItem->n; i++){
      int j = (*threadListItem->delivered)[i];
      if (threadListItem->URB->canDeliver(i, j+1)){
        pthread_mutex_lock(threadListItem->output_lock);
        threadListItem->output->push_back("d " + std::to_string(i+1)+ " " +  std::to_string(++(*threadListItem->delivered)[i]));
        pthread_mutex_unlock(threadListItem->output_lock);

      }
    }
  }
  //pthread_exit(0);
}

void FIFObroadcast::startReceiving(){
  int e = pthread_create(&broadcaster, NULL, thr_receiver, &(threadListItem));
  if(e==-1) {
     error("fifo_broadcast: pthread_create");
  }
}

void FIFObroadcast::stopReceiving(){
  int e = pthread_join(broadcaster, NULL);
  if(e==-1) {
     error("fifo_broadcast: pthread_join");
  }
  pthread_mutex_destroy(&output_lock);
}

void FIFObroadcast::printOutput(){
  for(auto it = this->output.begin(); it != this->output.end(); it++){
    fprintf(this->fout, "%s\n", (*it).c_str());
  }
  printf("PID: %d printed a vector of length %d\n",pid+1, (int)output.size() );
  URB->urbPrint();
}
