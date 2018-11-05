#include "perfectLink.hpp"
#include <vector>
#include <string>
#include <map>

typedef void(*RBMessageCallback)(const char *ip, char *data, int datalength);


typedef struct {
	int port;
	RBMessageCallback callback;
} RBThreadList;

class reliableBroadcast{
public:
  reliableBroadcast(int n);

  void broadcast(string data);
  void receiver();
  bool canDeliver();


private:
	void pp2pCallback(const char * ip, char * data, int datalength);
	int n; //num of Procsess
	perfectLink link;
	unsigned int m;
	std::vector<std::string> delivery;
	std::map<char,bool> delivered;
	std::vector<bool> forward;
	std::vector<bool> correct;
	std::map<char, int> ack;


}

