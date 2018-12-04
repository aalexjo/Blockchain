#include "reliableBroadcast.hpp"
#include <functional>

//this function uses stupidly inefficient datastructurs and initializez lots of stuff in runtime
//it is therefore very slow and the main source of delay in the entire project
//if you feel like you have a few minutes on your hands optmize this
void *thr_acker(void * arg) {
  struct timespec sleep_time;
  sleep_time.tv_sec = 0;
  sleep_time.tv_nsec = 1;
  nanosleep(&sleep_time, NULL);//can wait here if we want to let the broadcaster get a head start
  URBThreadList * threadListItem = (URBThreadList*) arg;

  msg_s* ack_msg;
  ack_msg = new msg_s;
  ack_msg->is_ack = 1;
  ack_msg->ack_from = threadListItem->pid;
  unsigned int i = 0;
  while(true){
    if(i < threadListItem->received->size()){
      //printf("size of received %d\n", threadListItem->received->size());
      pthread_mutex_lock(threadListItem->received_lock);
      msg_s msg = threadListItem->received->at(i);//badly need to remove this message from received
      pthread_mutex_unlock(threadListItem->received_lock);

      i++;
      if(threadListItem->ack->at(msg.sender).find(msg.seq_nr) == threadListItem->ack->at(msg.sender).end()){//have not seen this message before
        threadListItem->ack->at(msg.sender)[msg.seq_nr] = std::vector<int>(1,threadListItem->pid);
        ack_msg->seq_nr = msg.seq_nr;
        ack_msg->sender = msg.sender;

        threadListItem->link->broadcast(ack_msg);

      }
      if(msg.is_ack){
        for(unsigned int i = 0; i < threadListItem->ack->at(msg.sender)[msg.seq_nr].size(); i++){
          if(threadListItem->ack->at(msg.sender)[msg.seq_nr][i] == msg.ack_from){
            //printf("already acked on scr:%d sn:%d, from %d\n", msg.sender, msg.seq_nr,msg.ack_from);
            continue;
          }
        }
        threadListItem->ack->at(msg.sender)[msg.seq_nr].push_back(msg.ack_from);
        //printf("received acknowlagement for %d with seq_nr %d from %d\n", msg.sender, msg.seq_nr, msg.ack_from);
      }

    }else{
      //wait a bit to avoid an empty loop
  		sleep_time.tv_sec = 0;
  		sleep_time.tv_nsec = 100;
  		nanosleep(&sleep_time, NULL);
    }
  }

  return 0;//this function will never return

}

reliableBroadcast::reliableBroadcast(int n, int pid, std::vector<int> ports): n(n),  pid(pid), forward(n), ack(n){
  seq_nr = 0;
  using namespace std::placeholders;
  link = new PerfectLink(pid, ports, std::bind(&reliableBroadcast::pp2pCallback, this, _1));
  link->startReceiving();
  pthread_mutex_init(&(this->received_lock),NULL);

  this->threadListItem.link = this->link;
  this->threadListItem.ack = &(this->ack);
  this->threadListItem.received = &(this->received);
  this->threadListItem.pid = pid;
  this->threadListItem.received_lock = &received_lock;

  pthread_t listener;
	int e = pthread_create(&listener, NULL, thr_acker, &(this->threadListItem));
	if (e == -1)  error("reliableBroadcast: pthread_create");
}



void reliableBroadcast::broadcast(struct msg_s* msg){
  //this->forward[pid].push_back(msg->seq_nr);
  //this->ack[pid][msg->seq_nr].push_back(pid);
  link->broadcast(msg);
}


//essential that callback function is fast as this is called from UDPreceiver
void reliableBroadcast::pp2pCallback(struct msg_s* msg) {
  msg_s new_message = {msg->seq_nr, msg->sender, msg->is_ack, msg->ack_from};
  pthread_mutex_lock(&(this->received_lock));
  received.push_back(new_message);
  pthread_mutex_unlock(&(this->received_lock));
}

bool reliableBroadcast::canDeliver(int pi_sender, int m){
  if(this->ack[pi_sender].find(m) != this->ack[pi_sender].end()){
	      if (this->ack[pi_sender][m].size() > (unsigned int)n/2) {
		     return true;
	      }
      }
	return false;
}

void reliableBroadcast::urbPrint(){
//  printf("");
  link->linkPrint();
}
