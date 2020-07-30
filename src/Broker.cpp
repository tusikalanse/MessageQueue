#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include "Broker.h"

Broker::Broker() : 
    nextMessageID(0),
    refreshTimeout(1), 
    messageQueue(), 
    table(), 
    port(8000),
    subscription()
  {}

Broker::Broker(in_port_t _port) : 
    nextMessageID(0),
    refreshTimeout(1), 
    messageQueue(), 
    table(), 
    port(_port),
    subscription()
  {}

Broker::Broker(in_port_t _port, int priority) : 
    nextMessageID(0),
    refreshTimeout(1), 
    messageQueue(priority), 
    table(), 
    port(_port),
    subscription()
  {}

Broker::Broker(in_port_t _port, int priority, int _refreshTimeout) : 
    nextMessageID(0),
    refreshTimeout(_refreshTimeout), 
    messageQueue(priority), 
    table(), 
    port(_port), 
    subscription() 
  {}

void Broker::run() {
  int server_sockfd;
  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  
  if ((server_sockfd = socket(PF_INET,SOCK_STREAM,0)) < 0) {  
    perror("socket error");
    return;
  }

  if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind error");
    return;
  }

  if (listen(server_sockfd, 128) < 0) {
    perror("listen error");
    return;
  }

  std::thread listener(&Broker::server, this, server_sockfd);
  listener.detach();

  while (1) {
    if (messageQueue.empty()) {
      //sleep(1);
      //std::cout << "empty message queue" << std::endl;
      continue;
    }
    //std::cout << "message queue size is " << messageQueue.size() << std::endl;
    std::cout << "nextMessageID is " << nextMessageID << std::endl;
    std::shared_ptr<Message> message = messageQueue.top();
    messageQueue.pop();
    // TODO : ACK message
    sendMessage(message);
  }
}

void Broker::server(int server_sockfd) {
  int client_sockfd;
  sockaddr_in client_addr;
  unsigned int sin_size = sizeof(client_addr);

  while (1) {
    //std::cout << "Waiting for connection" << std::endl;
    int client_fd = accept(server_sockfd, (struct sockaddr *)&client_addr, &sin_size);
    socketTable[nextUserID++] = client_fd;
    std::thread client(&Broker::work, this, client_fd);
    client.detach();
  }
}

void Broker::work(int client_sockfd) {
  send(client_sockfd, "Welcome to my MQ!\n", 18, 0);
  int len;
  char buf[BUFSIZ];
  //std::cout << "Connection established" << std::endl;
  while ((len = recv(client_sockfd, buf, BUFSIZ, 0)) > 0) {
    buf[len] = '\0';
    //std::cout << buf << std::endl;
    messageQueue.push(getMessage(buf));
    //std::cout << "KJFDJSLKJFDKLJ" << std::endl;
  }
  //std::cout << "QUIT" << std::endl;
}

std::shared_ptr<Message> Broker::getMessage(char str[]) {
  return std::make_shared<Message>(Message(0, rand() % 10, str, nextMessageID++));
}

void Broker::sendMessage(std::shared_ptr<Message> message) {
  for (std::pair<int, bool> UserID : subscription.getUsers(message->topic)) {
    int clientID = socketTable[UserID.first];
    if (send(clientID, message->message.c_str(), message->message.size(), 0) < 0) {
      perror("write error");
      return;
    }
  }
}