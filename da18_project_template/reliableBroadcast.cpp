#include "reliableBroadcast.hpp"


reliableBroadcast::reliableBroadcast(int n, int pid):n(n), pid(pid){
  //delivered = empty;
  //pending = empty;
  for(int i = 0; i < n; i++){
    correct.push_back(true); //assuming the processes are 0-n, else need to insert them explisitly. Perhaps in a map
    //ack = empty
  }
  /*
  auto it = correct.begin();
  correct.insert(it,m,true);*/
}

void reliableBroadcast::broadcast(char const * data, int dataLength){

  pending.push_back();
  link.broadcast(data, datalength);

}

void reliableBroadcast::receiver(){

}

bool reliableBroadcast::canDeliver(int m){
  return ack[m]>this->n/2;
}
