#include "reliableBroadcast.hpp"
#include <functional>

//this function uses some inefficient datastructurs and initializez stuff in runtime
//it is therefore very slow and a large source of delay in the  project
//if you feel like you have a few minutes on your hands optmize this

//update, fixed a couple of things, but it is still kind of slow
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
      pthread_mutex_lock(threadListItem->ack_lock);
      if(threadListItem->ack->at(msg.sender).size() <= msg.seq_nr){//have not seen this messages seq_nr yet
        (threadListItem->ack->at(msg.sender)).resize((int)msg.seq_nr+1,std::vector<char>(threadListItem->n, 0));
      }

      if((threadListItem->ack->at(msg.sender))[msg.seq_nr][threadListItem->pid] == 0){//ack the message if we havent done so
        threadListItem->ack->at(msg.sender)[msg.seq_nr][threadListItem->pid] = 1;
        ack_msg->seq_nr = msg.seq_nr;
        ack_msg->sender = msg.sender;
        threadListItem->link->broadcast(ack_msg);
      }else if(!msg.is_ack){//re ack if this is a real message in case they are resending because a ack was lost
        ack_msg->seq_nr = msg.seq_nr;
        ack_msg->sender = msg.sender;
        threadListItem->link->broadcast(ack_msg);
      }

      if(msg.is_ack){
        (threadListItem->ack->at(msg.sender))[msg.seq_nr][msg.ack_from] = 1;
        /*this would be a nice way to reduce overhead, but sadly does not work
        and i do not want to spend more time on figuring it out

        // if(msg.sender == threadListItem->pid){
        //   unsigned int num_acks = 0;
        //   for(unsigned int i = 0; i < threadListItem->ack->at(msg.sender)[msg.seq_nr].size(); i++){
        //     if(threadListItem->ack->at(msg.sender)[msg.seq_nr][i]) num_acks++;
        //   }
        //   if (num_acks > threadListItem->ack->at(msg.sender)[msg.seq_nr].size()/2) {
        //     threadListItem->link->stopBroadcast(msg.seq_nr);
        //   }
        // }
        */
      }
      pthread_mutex_unlock(threadListItem->ack_lock);

    }else{
      //wait a bit to avoid an empty loop
  		sleep_time.tv_sec = 0;
  		sleep_time.tv_nsec = 100;
  		nanosleep(&sleep_time, NULL);
    }
  }

  return 0;//this function will never return

}

reliableBroadcast::reliableBroadcast(int n, int pid, std::vector<int> ports, int message_n): n(n),  pid(pid), forward(n), ack(n){
  seq_nr = 0;
  using namespace std::placeholders;
  link = new PerfectLink(pid, ports, std::bind(&reliableBroadcast::pp2pCallback, this, _1));
  link->startReceiving();
  pthread_mutex_init(&(this->received_lock),NULL);
  pthread_mutex_init(&(this->ack_lock),NULL);
  ack = std::vector<std::vector<std::vector<char>>>(n, std::vector<std::vector<char>>(1, std::vector<char>(n,0)));

  this->threadListItem.link = this->link;
  this->threadListItem.ack = &(this->ack);
  this->threadListItem.received = &(this->received);
  this->threadListItem.pid = pid;
  this->threadListItem.received_lock = &received_lock;
  this->threadListItem.ack_lock = &ack_lock;
  this->threadListItem.n = n;

  pthread_t listener;
	int e = pthread_create(&listener, NULL, thr_acker, &(this->threadListItem));
	if (e == -1)  error("reliableBroadcast: pthread_create");
}



void reliableBroadcast::broadcast(struct msg_s* msg){
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
  pthread_mutex_lock(&(this->ack_lock));
  if(this->ack[pi_sender].size() > (unsigned int)m){
    int num_acks = 0;
    for(unsigned int i = 0; i < this->ack[pi_sender][m].size(); i++){
      if(this->ack[pi_sender][m][i]) num_acks++;
    }
    if (num_acks > n/2) {
      pthread_mutex_unlock(&(this->ack_lock));
     return true;
   }
  }
  pthread_mutex_unlock(&(this->ack_lock));
	return false;
}

void reliableBroadcast::urbPrint(){
  link->linkPrint();
}
