#include "client.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

void Client::display(void) {
  cout << "Process number : " << process_i << '\n';
  for (int i = 0; i < process_n; i++) {
    cout << id[i] << ":" << ips[i] << ":" << ports[i] << '\n';
  }
}
