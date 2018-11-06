#include "reliableBroadcast.hpp"
#include <functional>

reliableBroadcast::reliableBroadcast(int n, int pid, std::vector<int> ports): n(n),  pid(pid), forward(n), ack(n){
  seq_nr = 0;
  using namespace std::placeholders;
  link = new PerfectLink(pid, ports, std::bind(&reliableBroadcast::pp2pCallback, this, _1));
  link->startReceiving();

}



void reliableBroadcast::broadcast(struct msg_s* msg){
  this->forward[pid].push_back(msg->seq_nr);
  //this->ack[pid][msg->seq_nr].push_back(pid);
  link->broadcast(msg);
}



void reliableBroadcast::pp2pCallback(struct msg_s* msg) {
  if(ack[msg->sender].find(msg->seq_nr) == ack[msg->sender].end()){//have not seen this message before
    ack[msg->sender][msg->seq_nr] = std::vector<int>(1,pid);
    static msg_s a = {msg->sender, msg->seq_nr, true, pid};
    link->broadcast(&a);
    printf("received new message from %d with seq_nr %d \n", a.sender, a.seq_nr);
  }else{
    if(msg->is_ack){
      for(int i = 0; i < ack[msg->sender][msg->seq_nr].size(); i++){
        if(ack[msg->sender][msg->seq_nr][i] == msg->ack_from){
          printf("already acked on scr:%d sn:%d, from %d\n", msg->sender, msg->seq_nr,msg->ack_from);
          return;
        }
      }
      ack[msg->sender][msg->seq_nr].push_back(msg->ack_from);
      printf("received acknowlagement for %d with seq_nr %d from %d\n", msg->sender, msg->seq_nr, msg->ack_from);

    }
  }


}
/*    if (msg->is_ack == true){
      if(ack[msg->sender][msg->seq_nr].empty()){
        printf("here %d\n", msg->sender );
        ack[msg->sender][msg->seq_nr].push_back(msg->ack_from);
      }else{
        for( auto it = ack[msg->sender][msg->seq_nr].begin(); it != ack[msg->sender][msg->seq_nr].end(); ++it){ //lol this looks bad
          if (*it == msg->ack_from){//Have we already recived this ack before, pp2pl might redeliver
            //printf("process %d already received ack from %d on message number %d\n",pid, msg->sender, msg->seq_nr);
            return;
          }
          ack[msg->sender][msg->seq_nr].push_back(msg->ack_from);
        }
      }
    }

    for(auto it = forward[msg->sender].rbegin(); it != forward[msg->sender].rend(); ++it){//iterate backwards because it is likly further back
      if (*it == msg->seq_nr){//have we already acked this message?
        return;
      }
    }
    forward[msg->sender].push_back(msg->seq_nr);
    msg->is_ack = true;
    msg->ack_from = pid; //pid is 0 indexed
    link->broadcast(msg);
}*/

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
  //pi_sender--;
  //printf("can we deliver msg: %d from sender: %d\n", m, pi_sender );

  if(this->ack[pi_sender].find(m) != this->ack[pi_sender].end()){
	      if (this->ack[pi_sender][m].size() > 0) {
		     return true;
	      }
      }
	return false;
}
