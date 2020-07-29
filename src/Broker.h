#ifndef BROKER_H
#define BROKER_H

#include <cstdio>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include "MessageTable.h"
#include "MyMessageQueue.h"
#include "Subscription.h"

class Broker {
 public:
  Broker();
  Broker(in_port_t _port);
  Broker(in_port_t _port, int priority);
  Broker(in_port_t _port, int priority, int _refreshTimeout);
  void run();
  void server(int server_sockfd);
  void work(int client_sockfd);
  std::shared_ptr<Message> getMessage(char str[]);
  void sendMessage(std::shared_ptr<Message> message);
 private:
  int nextUserID;
  int nextMessageID;
  const in_port_t port;
  const int refreshTimeout;
  MyMessageQueue messageQueue;
  __gnu_pbds::gp_hash_table<int, int> socketTable;
  MessageTable table; 
  Subscription subscription;
};

#endif
