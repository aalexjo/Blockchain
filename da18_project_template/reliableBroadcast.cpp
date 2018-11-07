#include "reliableBroadcast.hpp"
#include <functional>

reliableBroadcast::reliableBroadcast(int n, int pid, std::vector<int> ports): n(n),  pid(pid), forward(n), ack(n){
  seq_nr = 0;
  using namespace std::placeholders;
  link = new PerfectLink(pid, ports, std::bind(&reliableBroadcast::pp2pCallback, this, _1));
  link->startReceiving();
  this->ack_msg = new msg_s;
  ack_msg->is_ack = 1;
  ack_msg->ack_from = pid;
}



void reliableBroadcast::broadcast(struct msg_s* msg){
  this->forward[pid].push_back(msg->seq_nr);
  //this->ack[pid][msg->seq_nr].push_back(pid);
  link->broadcast(msg);
}



void reliableBroadcast::pp2pCallback(struct msg_s* msg) {
  if(ack[msg->sender].find(msg->seq_nr) == ack[msg->sender].end()){//have not seen this message before
    ack[msg->sender][msg->seq_nr] = std::vector<int>(1,pid);
    ack_msg->seq_nr = msg->seq_nr;
    ack_msg->sender = msg->sender;

    link->broadcast(ack_msg);

    //printf("received new message from %d with seq_nr %d \n", msg->sender, msg->seq_nr);
  }else{
    if(msg->is_ack){
      for(unsigned int i = 0; i < ack[msg->sender][msg->seq_nr].size(); i++){
        if(ack[msg->sender][msg->seq_nr][i] == msg->ack_from){
          //printf("already acked on scr:%d sn:%d, from %d\n", msg->sender, msg->seq_nr,msg->ack_from);
          return;
        }
      }
      ack[msg->sender][msg->seq_nr].push_back(msg->ack_from);
      //printf("received acknowlagement for %d with seq_nr %d from %d\n", msg->sender, msg->seq_nr, msg->ack_from);
    }
  }
}


/*
|-this block might not be needed if FIFO broadcast ends up polling this module
|-but might be useful if we need to use a callback function

void *thr_listener(void * arg) {

}

void reliableBroadcast::receiver(){

	pthread_t listener;
	int e = pthread_create(&listener, NULL, thr_listener, &(this->threadListItem));
	if (e == -1)  error("reliableBroadcast: pthread_create");
}*/

bool reliableBroadcast::canDeliver(int pi_sender, int m){
  if(this->ack[pi_sender].find(m) != this->ack[pi_sender].end()){
	      if (this->ack[pi_sender][m].size() > (unsigned int)n/2) {
		     return true;
	      }
      }
	return false;
}
