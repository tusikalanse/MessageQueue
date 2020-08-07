#ifndef NETWORK_H
#define NETWORK_H

#include "Client.h"

namespace network {
  void setnonblocking(int sockfd);
  int read(Client& client);
  int send(Client& client, const char* buf, int len);
  //void setOutput(int sockfd, bool enableOutput);
  void HTTPParser(Client& client);
  int dealGet(const char* buf, int len);
  int dealPost(const char* buf, const char* body, int len);
  int dealPut(int UserID, const char* buf, const char* body, int len);
  int dealDelelte(const char* buf, const char* body, int len);
  int findIndex(const char* buf); 
  static const char API[] = "HTTP/1.1 200 OK\r\nContent-Length: 25\r\n\r\n{\t\"post su\"}</html>";
}

#endif /* NETWORK_H */
