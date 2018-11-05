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
  reliableBroadcast(int n);

  void broadcast(struct msg_s* msg);
  void receiver();
  bool canDeliver();

private:
	void pp2pCallback(struct msg_s* msg);//const char * ip, char * data, int datalength);
	int n; //num of Procsess
  int pid;
	perfectLink link;
	unsigned int seq_nr;
	std::vector<msg_s> delivered;
	std::vector<vector<int>> forward;
	std::map<int, vector<int>> ack;


}
