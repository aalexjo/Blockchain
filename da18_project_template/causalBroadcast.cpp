#include "causalBroadcast.hpp"



causalBroadcast::causalBroadcast(int n, int pid, std::vector<int> ports, int message_n, std::vector<int> dependencies):n(n), pid(pid), message_n(message_n), delivered(n, 1){
  this->dependencies = dependencies;
  VC =new int*[n];
  for(int i = 0; i<n; i++){
    VC* = new int[n]();
  }

  URB = new reliableBroadcast(n, pid, ports, message_n);
  std::string fname = "da_proc_" + std::to_string(pid+1) + ".out";
  fout = fopen(fname.c_str(), "w+");
  pthread_mutex_init(&(this->output_lock),NULL);

  threadListItem.URB = URB;
  threadListItem.n = n;
  threadListItem.delivered = &delivered;
  threadListItem.output = &output;
  threadListItem.output_lock = &output_lock;

}

void causalBroadcast::broadcast(struct msg_s* msg){
  output.push_back("b " +  std::to_string(msg->seq_nr));
  output.push_back("d " + std::to_string(i+1)+ " " +  std::to_string(msg->seq_nr));
  //TODO: broadcast with VC
  VC[n][n]++;
}

void *thr_receiver(void *arg) {
  CBThreadList * threadListItem = (CBThreadList*) arg;
  bool delivering;
  while(true){ //should we ever terminate?
    delivering = false;
    for(int p = 0; p<threadListItem->n; p++){
      int m = (*threadListItem->delivered)[p];
      msg_s *msg = threadListItem->URB->canDeliver(p, m);
      if (msg != NULL){//event URB deliver

        pthread_mutex_lock(threadListItem->output_lock);
        threadListItem->output->push_back("d " + std::to_string(p+1)+ " " +  std::to_string((*threadListItem->delivered)[p]));
        pthread_mutex_unlock(threadListItem->output_lock);
        delivering = true;
      }
    }
    if(!delivering){
      struct timespec sleep_time;
      sleep_time.tv_sec = 0;
      sleep_time.tv_nsec = 100000;//these values are modifiable
      nanosleep(&sleep_time, NULL);//no new messages to deliver, wait a bit before checking again
    }
  }
  printf("exiting thread\n");
  pthread_exit(0);
  return 0;
}

void causalBroadcast::startReceiving(){
  int e = pthread_create(&receiver, NULL, thr_receiver, &(threadListItem));
  if(e==-1) {
     error("fifo_broadcast: pthread_create");
  }
}

void causalBroadcast::stopReceiving(){


}

void causalBroadcast::printOutput(){
  for(auto it = this->output.begin(); it != this->output.end(); it++){
    fprintf(this->fout, "%s\n", (*it).c_str());
  }
  printf("PID: %d printed a vector of length %d\n",pid+1, (int)output.size() );
}
