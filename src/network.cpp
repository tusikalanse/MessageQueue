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
    int ret = recv(sockfd, buf, 1024, 0);
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
      res += ret;
      //printf("%d %d: %s\n", ret, strlen(buf), buf);
      //test 
      // puts("sending back");
      // ::send(sockfd, buf, strlen(buf), 0);
    }
  }
  return res;
}

int network::send(int sockfd, const char* buf, int len) {
  int pos = 0;
  while (pos < len) {
    int ret = ::send(sockfd, buf + pos, len - pos, 0);
    if (-1 == ret) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
        ret = 0;
        continue;
      }
      break;
    }
    pos += ret;
  }
  return pos;
}
