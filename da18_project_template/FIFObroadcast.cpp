#include "FIFObroadcast.hpp"
#include <iostream>

FIFObroadcast::FIFObroadcast(int n, int pid, std::vector<int> ports): URB(n, pid, ports), n(n), delivered(n){

}

void FIFObroadcast::broadcast(struct msg_s* msg){
  URB.broadcast(msg);
  std::cout<<"b "<<msg->seq_nr<<std::endl;
}

void FIFObroadcast::startReceiving(){

  while(true){
    for(int i = 0; i<=n; i++){
      if (URB.canDeliver(i, delivered[i]+1)){
        //TODO:deliver that shit
        std::cout<<"d "<< i << ++delivered[i]<< std::endl;
      }
    }

  }
}
