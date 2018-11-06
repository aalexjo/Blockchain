#include <sys/socket.h>
#include <netinet/in.h>

#include <cassert>
#include <vector>
#include <string>

using namespace std;

enum Layer { PL, BEB, URB, FIFO };
struct req {
  int dst;
  int src;
  int seq_nbr;
  Layer layer;
};

class Client {
private :
  sockaddr_in self_addr;
  int seq_nbr = 0;
  int process_i;
  int process_n;
  int message_n;
  vector <int> id;
  vector <string> ips;
  vector <int> ports;
  vector< vector<int> > delivered;

public :
  Client(int process_i, int process_n, int message_n, vector <int> id, vector <string> ips, vector <int> ports);
  void display(void);
  void startReceiving(void);
  void broadcast(void);
  void sendto_req_udp(int dest_p, int sockfd);
};
