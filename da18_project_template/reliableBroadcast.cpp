#include "reliableBroadcast.hpp"



reliableBroadcast::reliableBroadcast(int n): link(){
	this->n = n;
}

void reliableBroadcast::broadcast(string data){
	data += this->m;
	const char* msg = data.c_str();
	perfectLink.broadcast(msg, strlen(msg));
	this->m++;
}

void *thr_listener(void * arg) {



}

void pp2pCallback(const char * ip, char * data, int datalength) {
	std::string msg(data);

	if (msg[0] == 'a') {
		ack[msg[1]]++;
	}
	else {
		if (msg.back)
	}
}



void reliableBroadcast::receiver(){

	pthread_t listner;
	int e = pthread_create(&listener, NULL, thr_listener, &(this->threadListItem));
	if (e == -1)  error("reliableBroadcast: pthread_create");
}

bool reliableBroadcast::canDeliver(unsigned int m){
	if (ack[m] > n / 2) {
		return true;
	}
	return false;
}
