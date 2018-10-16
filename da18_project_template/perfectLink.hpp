
typedef struct{
  UDP udp;
  char const *data;
  int datalength;
} perfectLinkThreadList;

class perfectLink{
public:
  perfectLink(char const *addr, int port, perfectLinkMessageCallback callback);
  void broadcast(char const *data, int datalength);
  void startReceiving();
private:
  std::vector<string> delivered;
  int messagesSent;
  string processID;
};
