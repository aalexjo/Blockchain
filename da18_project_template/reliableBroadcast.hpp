#include "perfectLink.hpp"
#include <vector>
#include <string>
#include <map>

//typedef void(*RBMessageCallback)(const char *ip, char *data, int datalength);


typedef struct {
	int port;
	RBMessageCallback callback;
} RBThreadList;

class reliableBroadcast{
public:
  reliableBroadcast(int n, int pid, std::vector<int> ports);

  void broadcast(struct msg_s* msg);
  void receiver();
  bool canDeliver(int pi_src. int m);

private:
	void pp2pCallback(struct msg_s* msg);
	int n; //num of Procsess
  int pid;
	perfectLink link;
	unsigned int seq_nr;
	std::vector<msg_s> delivered;
	std::vector<vector<int>> forward;
	std::vector<std::map<int, vector<int>>> ack; //ack[src_pi][seq_nr][acking_pi]


}
