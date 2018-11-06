#include "reliableBroadcast.hpp"
#include <functional>

reliableBroadcast::reliableBroadcast(int n, int pid, std::vector<int> ports): forward(n), ack(n){
	this->n = n;
  seq_nr = 0;
  link = new PerfectLink(n, ports, pp2pCallback);
  link->startReceiving();

}

void reliableBroadcast::broadcast(struct msg_s* msg){
  this->forward[pid].push_back(msg->seq_nr);
  link->broadcast(msg);
}



void reliableBroadcast::pp2pCallback(struct msg_s* msg) {
    if (msg->is_ack == true){
        for( auto it = this->ack[msg->sender][msg->seq_nr].begin(); it != this->ack[msg->sender][msg->seq_nr].end(); ++it){ //lol this looks bad
          if (*it == msg->ack_from){//Have we already recived this ack before, pp2pl might redeliver
            return;
          }
          this->ack[msg->sender][msg->seq_nr].push_back(msg->ack_from);
        }
    }

    for(auto it = this->forward[msg->sender].rbegin(); it != this->forward[msg->sender].rend(); ++it){//iterate backwards because it is likly further back
      if (*it == msg->seq_nr){//have we already acked this message?
        return;
      }
    }
    this->forward[msg->sender].push_back(msg->seq_nr);
    msg->is_ack = true;
    msg->ack_from = this->pid;
    this->link->broadcast(msg);
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
	if (this->ack[pi_sender][m].size() > n / 2) {
		return true;
	}
	return false;
}
