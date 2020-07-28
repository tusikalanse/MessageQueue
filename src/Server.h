#ifndef SERVER_H
#define SERVER_H

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Server {
 public:
  Server(int _port);
  int run();
  static void work(int client_sockfd);
 private:
  const in_port_t port;
};


#endif
