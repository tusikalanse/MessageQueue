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

int network::read(Client& client) {
  int sockfd = client.socketID;
  char* buf = client.buf + client.readIDX;
  int res = 0;
  while (1) {
    int ret = recv(sockfd, buf, BUFSIZ - client.readIDX - 10, 0);
    if (ret < 0) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
        break;
      }
      close(sockfd);
      return 0;
    }
    else if (ret == 0) {
      close(sockfd);
      return 0;
    }  
    else {
      buf[ret] = '\0';
      res += ret;
      //printf("%d %d: %s\n", ret, strlen(buf), buf);
      //fflush(stdout);
    }
  }
  return res;
}

int network::send(Client& client, const char* buf, int len) {
  int sockfd = client.socketID;
  int pos = 0;
  while (pos < len) {
    int ret = ::send(sockfd, buf + pos, len - pos, 0);
    if (-1 == ret) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
        ret = 0;
        continue;
      }
      return 0;
    }
    pos += ret;
  }
  return pos;
}

void network::HTTPParser(Client& client) {

}