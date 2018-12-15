#include "causalBroadcast.hpp"



causalBroadcast::causalBroadcast(int n, int pid, std::vector<int> ports, int message_n, std::vector<bool> dependencies):n(n), pid(pid), message_n(message_n), delivered(n, 1){
  this->dependencies = dependencies;
  VC =new int[n]();
  // for(int i = 0; i<n; i++){
  //   VC* = new int[n]();
  // }

  URB = new reliableBroadcast(n, pid, ports, message_n);
  std::string fname = "da_proc_" + std::to_string(pid+1) + ".out";
  fout = fopen(fname.c_str(), "w+");
  pthread_mutex_init(&(this->output_lock),NULL);
  pthread_mutex_init(&(this->VC_lock),NULL);

  threadListItem.URB = URB;
  threadListItem.n = n;
  threadListItem.delivered = &delivered;
  threadListItem.output = &output;
  threadListItem.output_lock = &output_lock;
  threadListItem.VC_lock = &VC_lock;
  threadListItem.VC = VC;
  threadListItem.pid = pid;
  threadListItem.dependencies = dependencies;
}

void causalBroadcast::broadcast(struct msg_s* msg){
  pthread_mutex_lock(&(this->output_lock));
  output.push_back("b " +  std::to_string(msg->seq_nr));
  //output.push_back("d " + std::to_string(pid+1)+ " " +  std::to_string(msg->seq_nr));
  pthread_mutex_unlock(&(this->output_lock));

  //TODO: broadcast with VC
  pthread_mutex_lock(&(this->VC_lock));
  msg->VC=VC;
  URB->broadcast(msg);
  pthread_mutex_unlock(&(this->VC_lock));

  //VC[pid]++;
}

void *thr_CBreceiver(void *arg) {
  CBThreadList * threadListItem = (CBThreadList*) arg;
  bool delivering;
  while(true){ //should we ever terminate?
    delivering = false;
    for(int p = 0; p<threadListItem->n; p++){
      int m = (*threadListItem->delivered)[p];
      msg_s *msg = threadListItem->URB->canDeliver(p, m);
      if (msg != NULL){//event URB deliver
        bool causalOrder = true;
        for(int i = 0; i < threadListItem->n; i++){
          if(!(threadListItem->VC[i] >= msg->VC[i]) && threadListItem->dependencies[i]){
            if(threadListItem->pid == 0) printf("process %d is unable to deliver %d %d because our VC is %d and the message VC is %d for p%d\n", threadListItem->pid+1, p+1, m,threadListItem->VC[i], msg->VC[i], i+1);
            causalOrder = false;
          }
        }
        if(causalOrder){
          pthread_mutex_lock(threadListItem->output_lock);
          threadListItem->output->push_back("d " + std::to_string(p+1)+ " " +  std::to_string(m));
          pthread_mutex_unlock(threadListItem->output_lock);
          (*threadListItem->delivered)[p]++;
          delivering = true;
          pthread_mutex_lock(threadListItem->VC_lock);
          threadListItem->VC[p]++;
          pthread_mutex_unlock(threadListItem->VC_lock);
        }
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
  int e = pthread_create(&receiver, NULL, thr_CBreceiver, &(threadListItem));
  if(e==-1) {
     error("fifo_broadcast: pthread_create");
  }
}

void causalBroadcast::stopReceiving(){
  int e = pthread_join(receiver, NULL);
  printf("joined thread");
  if(e==-1) {
     error("ca_stopreceiving: pthread_join");
  }
  pthread_mutex_destroy(&output_lock);
}

void causalBroadcast::printOutput(){
  for(auto it = this->output.begin(); it != this->output.end(); it++){
    fprintf(this->fout, "%s\n", (*it).c_str());
  }
  printf("PID: %d printed a vector of length %d\n",pid+1, (int)output.size() );
}
