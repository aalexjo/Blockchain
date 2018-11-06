#include <sys/socket.h>
#include <netinet/in.h>

#include <cassert>
#include <vector>
#include <string>
#include <cstdio>
using namespace std;

struct msg_s {
  bool is_ack;
  int creator;
  int seq_nbr;
  int src;
};

class Client {
private :
  sockaddr_in self_addr;
  int seq_nbr = 0;
  int pid;
  int process_n;
  int message_n;
  vector <int> id;
  vector <string> ips;
  vector <int> ports;
  vector< vector< vector<bool> > > deliveredPL, ack;
  vector< vector< bool >  > forwarded, deliveredURB;
  vector<int> curr_head;
  void bebBroadcast(msg_s msg, int sockfd);
  void urbBroadcast(int seq_nbr, int sockfd);
  void sendto_udp(msg_s msg, int dst, int sockfd);
  void urbDeliverCheck(int creator, int seq_nbr);

  FILE *fout;

public :
  Client(int pid, int process_n, int message_n, vector <int> id, vector <string> ips, vector <int> ports);
  void display(void);
  void startReceiving(void);
  void broadcastMessages(void);
};
