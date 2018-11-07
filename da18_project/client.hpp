#include <sys/socket.h>
#include <netinet/in.h>

#include <cassert>
#include <vector>
#include <string>
#include <cstdio>
using namespace std;

enum msg_t {isMsg, isMsgAck, isDoneReq, isDoneAck};

struct msg_s {
  msg_t type;
  int creator;
  int seq_nbr;
  int src;
  bool done;
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
  vector< vector< vector<bool> > > deliveredPL, ackURB, ackPL;
  vector< vector< bool >  > forwarded, deliveredURB;
  vector<int> curr_head;
  vector<bool> done, ackDone;
  void bebBroadcast(msg_s msg);
  void urbBroadcast(int seq_nbr);
  void sendto_udp(msg_s msg, int dst, int sockfd);
  void urbDeliverCheck(int creator, int seq_nbr);

  FILE *fout;

public :
  Client(int pid, int process_n, int message_n, vector <int> id, vector <string> ips, vector <int> ports);
  void display(void);
  void startReceiving(void);
  void broadcastMessages(void);
};
