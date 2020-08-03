#ifndef NETWORK_H
#define NETWORK_H

namespace network {
  void setnonblocking(int sockfd);
  int read(int sockfd);
  int send(int sockfd, const char* buf, int len);
}

#endif /* NETWORK_H */
