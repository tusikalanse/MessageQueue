#include "network.h"

#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

void network::setnonblocking(int sockfd) {
  int flag = fcntl(sockfd, F_GETFL, 0);
  if (flag < 0) {
      perror("fcntl F_GETFL fail");
      return;
  }
  if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0) {
      perror("fcntl F_SETFL fail");
  }
}