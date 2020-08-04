#ifndef NETWORK_H
#define NETWORK_H

namespace network {
  void setnonblocking(int sockfd);
  int read(int sockfd, char* buf);
  int send(int sockfd, const char* buf, int len);
  void setOutput(int sockfd, bool enableOutput);
}

#endif /* NETWORK_H */
