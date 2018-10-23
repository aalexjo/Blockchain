#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "perfectLink.hpp"

perfectLink::perfectLink(const char* addr, int port, perfectLinkMessageCallback callback): udp(addr, port, callback) {
  messagesSent = 0;
  // this->udp = udp(addr, port, callback);
  // UDP udp(addr, port, callback);
}

void *thr_broadcaster(void *arg) {
  perfectLinkThreadList * threadListItem = (perfectLinkThreadList*) arg;
  char const *data = threadListItem->data;
  int datalength = threadListItem->datalength;
  for (int i = 0; i < 5; i++) {
    printf("Broadcast number %d\n", i);
    threadListItem->udp.broadcast(data, datalength);
  }
  // Mulig threaden må ha en teller for å vite hvor lenge den skal skal sende/
  // hvor mange ganger den skal sende
  return 0;
}

void perfectLink::broadcast(char const *data, int datalength){
  //TODO: her kan man implementere stubborn link sending ved å spawne en thread
  //som sender meldingen en gang per X antall sekunder
  perfectLinkThreadList threadListItem = {this->udp, data, datalength};
  pthread_t broadcaster;
  int e = pthread_create(&broadcaster, NULL, thr_broadcaster, &(threadListItem));
  if(e==-1) {
     error("perfectLink_broadcast: pthread_create");
  }
}

void perfectLink::startReceiving(){
  //TODO: have to make some deciens here about who should invoce UDP.startReceiving
  //somewhere the callbackfunction has to be implemented and handle receiving, should be here

  //TODO: find a way to trigger a deliver event to send it up the callstack. (polling or interrupts)
  //easiest mught just be to have a thread that is waiting for new messages to arrive

  // if (std::find(delivered.begin(), delivered.end(), "here should be message be, ass a string") != v.end()) {
    // Could not find the header of received message in our delivered vector,
    // Add it to delivered and deliver it to our processes


  // }
  this->udp.startReceiving();

  // -------------------------!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!----------------------
  // Mulig vi må joine p_threads for at de ska kunne kjøre skikkelig
}
