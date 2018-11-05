#include <cassert>
#include <vector>
#include <string>

using namespace std;

class Client {

private :
  int process_i;
  int process_n;
  int message_n;
  vector <int> id;
  vector <string> ips;
  vector <string> ports;

public :
  Client(int process_i, int process_n, int message_n, vector <int> id, vector <string> ips, vector <string> ports) :
    process_i(process_i), process_n(process_n),  message_n(message_n), id(id), ips(ips), ports(ports) { }
  void display(void);
  void broadcast(void);
};
