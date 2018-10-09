//kanskje noen headergaurds
typedef void (*UDPMessageCallback)(const char *ip, char *data, int datalength);

typedef struct{
  int port;
  UDPMessageCallback callback;
} UDPThreadList;

class UDP{
public:
  UDP(const char* ipaddr, int port, UDPMessageCallback callback);
  void broadcast(char const * data, int dataLength);
  void startReceiving();


private:
  //void *thr_listener(void* arg);
  UDPThreadList threadListItem;
  const char* ipaddr;
};
