#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string>
#include <arpa/inet.h>

typedef void (*UDPMessageCallback)(char * data, int datalength);

typedef struct{
  int port;
  UDPMessageCallback callback;
}

class UDP{
public:
  UDP(const char* ipaddr, int port /*TODO:callback function */): ipaddr(ipaddr), port(port){}
  void broadcast(char * data, int dataLength);
  void startReceiving(UDPMessageCallback function)


private:
  void *thr_listener(void* arg);
  const char* ipaddr;
  int port
  //UDPMessageCallback callback
}

class udp_listener{


}
