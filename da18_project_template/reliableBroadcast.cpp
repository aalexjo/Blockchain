#include "reliableBroadcast.hpp"

//Mostly done except delivery of message to above layer, has not been compiled and debugged yet


reliableBroadcast::reliableBroadcast(int n): link(addr, port, pp2pCallback), forward(n), ack(n){
	this->n = n;
  seq_nr = 0;
}

void reliableBroadcast::broadcast(struct msg_s* msg){
  forward[pid].push_back(msg->seq_nr);
  link.broadcast(msg);
}



void pp2pCallback(struct msg_s* msg) {
    if (msg->is_ack == true){
        for( auto it = ack[msg->src][msg->seq_nr].begin(); it != ack[msg->src][msg->seq_nr].end(); ++it){ //lol this looks bad
          if (*it == msg->ack_from){//Have we already recived this ack before, pp2pl might redeliver
            return;
          }
          ack[msg->src][msg->seq_nr].push_back(msg->ack_from);
        }
    }

    for(auto it = forward[msg->src].rbegin(); it != forward[msg->src].rend(); ++it){//iterate backwards because it is likly further back
      if (*it == msg.seq_nr){//have we already acked this message?
        return;
      }
    }
    forward[msg->src].push_back(msg->seq_nr);
    msg->is_ack = true;
    msg->ack_from = pid;
    link.broadcast(msg);
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

bool reliableBroadcast::canDeliver(int pi_src, int m){
	if (ack[pi_src][m].size() > n / 2) {
		return true;
	}
	return false;
}
