#ifndef NETWORK_H
#define NETWORK_H

#include "Client.h"

namespace network {
  void setnonblocking(int sockfd);
  int read(Client& client);
  int send(Client& client, const char* buf, int len);
  //void setOutput(int sockfd, bool enableOutput);
  void HTTPParser(Client& client);
  void RequestHandler(Client& client);
}

#endif /* NETWORK_H */
