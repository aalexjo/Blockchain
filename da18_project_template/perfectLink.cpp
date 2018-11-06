#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "perfectLink.hpp"

PerfectLink::PerfectLink(const char* addr, int port, perfectLinkMessageCallback callback): udp(addr, port, callback) {
  // messagesSent = 0;
  // this->udp = udp(addr, port, callback);
  // UDP udp(addr, port, callback);
}

//void PerfectLink::udpcallback(struct msg_s* msg) {
	// Assuming an ascii string here - a binary blob (including '0's) will
  // be ugly/truncated.
	// printf("Received UDP(perfect link) message from %d with seq number '%d'\n", msg->sender, msg->seq_nr);

 	// Mulig at dette må være i recive funksjoene til Perfect links
  // delivered.push_back(msg);
  // printf(delivered.size());
//}

void *thr_broadcaster(void *arg) {

	// printf("PerfectLink thread for broadcasting is running \n");

  perfectLinkThreadList * threadListItem = (perfectLinkThreadList*) arg;

  for (int i = 0; i < 20; i++) {
    // printf("Broadcast number %d\n", i);
  	threadListItem->udp.broadcast(threadListItem -> msg);
  }
  return 0;
}

void PerfectLink::broadcast(struct msg_s* msg){

	// printf("PerfectLink broadcast() \n");

  perfectLinkThreadList threadListItem = {this->udp, msg};
  pthread_t broadcaster;
  int e = pthread_create(&broadcaster, NULL, thr_broadcaster, &(threadListItem));
  if(e==-1) {
     error("perfectLink_broadcast: pthread_create");
  }


  // joining the thread, we may need to add a attr where we have the NULL in pthread_create!
/*
	int rc = pthread_join(broadcaster, NULL);
  if (rc) {
    // cout << "Error: unable to joine," << rc << endl;
    // exit(-1);
    error("pefectLink_broadcast: couldn't be joined");
  }
*/

}

void PerfectLink::startReceiving(){
  //TODO: have to make some deciens here about who should invoce UDP.startReceiving
  //somewhere the callbackfunction has to be implemented and handle receiving, should be here

  //TODO: find a way to trigger a deliver event to send it up the callstack. (polling or interrupts)
  //easiest mught just be to have a thread that is waiting for new messages to arrive

	// printf("PerfectLink startReceiving() \n");

  this->udp.startReceiving();

  //if (std::find(delivered.begin(), delivered.end(), "here should be message be, as a string") != v.end()) {
  //  Could not find the header of received message in our delivered vector,
  //  Add it to delivered and deliver it to our processes


  //}

  // -------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!----------------------
  // Mulig vi må joine p_threads for at de ska kunne kjøre skikkelig
}
