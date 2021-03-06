#ifndef BROKER_H
#define BROKER_H

#include <condition_variable>
#include <cstdio>
#include <mutex>
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
#include "PriorityMessage.h"
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
  //std::shared_ptr<Message> getMessage(char str[]);
  void sendMessage(std::shared_ptr<Message> message);
  void resendAll();
 private:
  static void setnonblocking(int sockfd);
  int read(Client& client);
  int send(Client& client, const char* buf, int len);
  //void setOutput(int sockfd, bool enableOutput);
  void HTTPParser(Client& client);
  int dealGet(const char* buf, int len);
  int dealPost(Client& client, const char* buf, const char* body, int len);
  int dealPut(Client& client, const char* buf, const char* body, int len);
  int dealDelete(Client& client, const char* buf, const char* body, int len);
  int findIndex(const char* buf); 
  void removeClient(int sockfd);
  //static const char API[] = "HTTP/1.1 200 OK\r\nContent-Length: 25\r\n\r\n{\t\"post su\"}</html>";
  std::mutex mutex_queue;
  //std::mutex mutex_IDTable;
  std::mutex mutex_socketTable;
  std::mutex mutex_subscription;
  std::mutex mutex_messageTable;
  std::condition_variable queue_isnot_empty;
  static const int MAX_EVENTS = 1000;
  static const int MAX_LEN = 1024;
  //test
  int cnt = 0, ccnt = 0, ACKTime = 0;
  int priorityNumber;
  int nextUserID = 0;
  int nextMessageID = 0;
  const in_port_t port;
  const int refreshTimeout;
  MyMessageQueue messageQueue;
  __gnu_pbds::gp_hash_table<int, Client> socketTable;
  __gnu_pbds::gp_hash_table<int, int> IDTable; //<sockfd, userid>
  MessageTable table; 
  Subscription subscription;
};

#endif /* BROKER_H */
