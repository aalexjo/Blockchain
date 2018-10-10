#include "UDP.hpp"
#include "stubbornLinks.hpp"
#include <pthread.h>


void perfectLink::Broadcast(char const *data, int datalength){
  //TODO: her kan man implementere stubborn link sending ved å spawne en thread
  //som sender meldingen en gang per X antall sekunder

}

void perfectLink::Receive(){
  //TODO: have to make some deciens here about who should invoce UDP.startReceiving
  //somewhere the callbackfunction has to be implemented and handle receiving, should be here

  //TODO: find a way to trigger a deliver event to send it up the callstack. (polling or interrupts)
  //easiest mught just be to have a thread that is waiting for new messages to arrive


}
