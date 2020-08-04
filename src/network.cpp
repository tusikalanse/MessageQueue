#include "network.h"

#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
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

int network::read(int sockfd, char* buf) {
  int res = 0;
  while (1) {
    int ret = recv(sockfd, buf, BUFSIZ, 0);
    res += ret;
    if (ret < 0) {
      if ( (errno == EAGAIN) || (errno == EWOULDBLOCK)) {
        break;
      }
      close(sockfd);
      break;
    }
    else if (ret == 0) {
      close(sockfd);
    }  
    else {
      buf[ret] = '\0';
      printf("%d %d: %s\n", ret, strlen(buf), buf);
    }
  }
  return res;
}