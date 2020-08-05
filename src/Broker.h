#ifndef BROKER_H
#define BROKER_H

#include <cstdio>
#include <queue>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include "Client.h"
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
  void server();
  void work(int client_sockfd);
  std::shared_ptr<Message> getMessage(char str[]);
  void sendMessage(std::shared_ptr<Message> message);
  void resendAll();
 private:
  static const int MAX_EVENTS = 1000;
  static const int MAX_LEN = 1024;
  int nextUserID = 0;
  int nextMessageID = 0;
  const in_port_t port;
  const int refreshTimeout;
  MyMessageQueue messageQueue;
  __gnu_pbds::gp_hash_table<int, Client> socketTable;
  __gnu_pbds::gp_hash_table<int, int> IDTable;
  MessageTable table; 
  Subscription subscription;
};

#endif /* BROKER_H */
