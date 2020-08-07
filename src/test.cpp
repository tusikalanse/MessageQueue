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

int main() {
  Broker server(8000);
  server.run();
  std::cout << "end" << std::endl;
}
