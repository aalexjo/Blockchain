#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include "UDP.hpp"
#include "perfectLink.hpp"


perfectLink::perfectLink(char const *addr, int port, perfectLinkMessageCallback callback) {
  messagesSent = 0;
  Protected UDP udp(addr, port, perfectLinkcallback);
}

void perfectLink::Broadcast(char const *data, int datalength){
  //TODO: her kan man implementere stubborn link sending ved å spawne en thread
  //som sender meldingen en gang per X antall sekunder
  perfectLinkThreadList threadListItem = {udp, data, datalength}
  pthread_t broadcaster;
  int e = pthread_create(&broadcaster, NULL, thr_broadcaster, &(this->threadListItem);
  if(e==-1)  error("perfectLink_broadcast: pthread_create");
}

void *thr_broadcaster(void *arg) {
  perfectLinkThreadList * threadListItem = (perfectLinkThreadList*) arg;
  char const *data = threadListItem->data;
  int datalength = threadListItem->datalength;
  threadListItem->udp.broadcast(data, datalength);

  // Mulig threaden må ha en teller for å vite hvor lenge den skal skal sende/
  // hvor mange ganger den skal sende
}

void perfectLink::Receive(){
  //TODO: have to make some deciens here about who should invoce UDP.startReceiving
  //somewhere the callbackfunction has to be implemented and handle receiving, should be here

  //TODO: find a way to trigger a deliver event to send it up the callstack. (polling or interrupts)
  //easiest mught just be to have a thread that is waiting for new messages to arrive

  // if (std::find(delivered.begin(), delivered.end(), "here should be message be, ass a string") != v.end()) {
    // Could not find the header of received message in our delivered vector,
    // Add it to delivered and deliver it to our processes


  // }
  upd.startReceiving();
}

void perfectLinkcallback(const char * ip, char * data, int datalength) {
  printf("Received UDP message from %s: '%s'\n",ip,data);
}
