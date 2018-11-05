#include <cassert>
#include <vector>
#include <string>

using namespace std;

struct msg {
  int sender;
  int receiver;
  int msg_idx;
} msg;

class Client {

private :
  int msg_idx;
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
  int sendto_udp(int process, msg* msg_);
  void recv_msg_udp(void);
