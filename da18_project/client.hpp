#include <cassert>
#include <vector>
#include <string>

using namespace std;

struct req {
  int dst;
  int src;
  int req_cnt;
};

class Client {
private :
  int req_cnt = 0;
  int process_i;
  int process_n;
  int message_n;
  vector <int> id;
  vector <string> ips;
  vector <int> ports;

public :
  Client(int process_i, int process_n, int message_n, vector <int> id, vector <string> ips, vector <int> ports) :
    process_i(process_i), process_n(process_n),  message_n(message_n), id(id), ips(ips), ports(ports) { }
  void display(void);
  void broadcast(void);
  void sendto_req_udp(struct req* req_);
  void recvfrom_req_udp(struct req* req_);
  void recv_req_udp(void);
};
