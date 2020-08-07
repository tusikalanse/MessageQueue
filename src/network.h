#ifndef NETWORK_H
#define NETWORK_H

#include "Client.h"

namespace network {
  void setnonblocking(int sockfd);
  int read(Client& client);
  int send(Client& client, const char* buf, int len);
  //void setOutput(int sockfd, bool enableOutput);
  void HTTPParser(Client& client);
  void dealGet(const char* buf, int len);
  void dealPost(const char* buf, const char* body, int len);
  void dealPut(const char* buf, const char* body, int len);
  void dealDelelte(const char* buf, const char* body, int len);
  int findIndex(const char* buf); 
}

#endif /* NETWORK_H */
