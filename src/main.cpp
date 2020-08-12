#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include "Broker.h"

#define MAX_EVENTS 1000
#define MAX_LEN 1024

int main(int argc, char **argv) {
  int priority = 0;
  if (argc != 1) {
    priority = atoi(argv[1]);
  }
  Broker server(8000, priority);
  server.run();
  std::cout << "end" << std::endl;
}


// #include "../src/MyPriorityQueue.h"
// using namespace std;

// int main() {
//   MyPriorityQueue pq;
//   pq.push(make_shared<Message>(PriorityMessage(1, 1, "1", 1, 2)));
//   pq.push(make_shared<Message>(PriorityMessage(1, 1, "1", 3, 444)));


//   return 0;
// }