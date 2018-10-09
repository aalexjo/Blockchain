#include "udp.hpp"


void UDP::broadcast(char * data, int dataLength){
  struct sockaddr_in si_other;
  int s, slen=sizeof(si_other);

  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) //setting up a UDP socket
    error("udp_send: socket");

  int broadcastEnable = 1;
  setsockopt(s,SOL_SOCKET,SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)); //enabling broadcast on this socket just in case

  memset((char *) &si_other, 0, sizeof(si_other)); //clearing si_other
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(this->port);
  if (inet_aton("255.255.255.255", &si_other.sin_addr)==0) {//setting destination address
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }

  int e = sendto(s, data, dataLength, 0, (struct sockaddr *) &si_other, slen);//sending data
  if(e==-1)  error("udp_broadcast: sendto()");

  close(s);//TODO: does closing the socket every time decrease performance? maybe open in an init function and save it in the class
}


void *thr_listener(void * arg){

  TUdpThreadListItem * threadListItem = (TUdpThreadListItem*) arg;

  struct sockaddr_in si_me, si_other;
  socklen_t slen = sizeof(si_other);
  char buf[BUFLEN];

  threadListItem->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  assert(threadListItem->socket != -1);

  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(threadListItem->port);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  int optval = 1;
  setsockopt(threadListItem->socket,SOL_SOCKET,SO_REUSEADDR, &optval, sizeof(optval));

  int res = bind(threadListItem->socket,(struct sockaddr *) &si_me, sizeof(si_me));
  if(res == -1) error("thr_udpListen:bind");

  if(m_log) printf("Sverresnetwork: udpListen: Binding to port %d\n",threadListItem->port);

  while(1){
    res = recvfrom(threadListItem->socket, buf, BUFLEN, 0,(struct sockaddr *) &si_other, &slen);
    if(res == -1) error("thr_udpListen:recvfrom");
    if(res >= BUFLEN-1){
      fprintf(stderr,"recvfrom: Hmm, length of received message is larger than max message length: %d vs %d\n\n",res,BUFLEN);
      assert(res < BUFLEN-1);
    }
    // printf("Received packet from %s:%d\nLength = %d, Data: <%s>\n\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), res,buf);
     (*(threadListItem->callBack))(inet_ntoa(si_other.sin_addr),buf,res);
  }

  // Never executed - this thread will be killed if it is not needed any more.
  close(threadListItem->socket);
  return 0;
}

void udp_startReceiving(int port,TMessageCallback callBack){

  pthread_t listener;
  int e = pthread_create(&listener, NULL, thr_listener, arg);
  if(e==-1)  error("udp_broadcast: pthread_create");
}
