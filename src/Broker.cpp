#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/epoll.h>
#include "Broker.h"
#include "network.h"

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
  std::thread listener(&Broker::server, this);
  listener.detach();

  std::thread ticker(&Broker::resendAll, this);
  ticker.detach();

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

void Broker::server() {
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

  struct epoll_event ev, events[MAX_EVENTS];
  int epollfd = epoll_create1(0);
  if (-1 == epollfd) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }
  ev.events = EPOLLIN; 
  ev.data.fd = server_sockfd;
  if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, server_sockfd, &ev)) {
    perror("epoll_ctl EPOLL_CTL_ADD fail");
    exit(EXIT_FAILURE);
  }

  int client_sockfd;
  sockaddr_in client_addr;
  unsigned int sin_size = sizeof(client_addr);
  while (1) {
    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (-1 == nfds) {
      perror("epoll_wait fail");
      exit(EXIT_FAILURE);
    }
    for (int n = 0; n < nfds; ++n) {
      if (events[n].data.fd == server_sockfd) {
        if (!(events[n].events & EPOLLIN)) continue;
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        int connfd = accept(server_sockfd, (sockaddr *)&cliaddr, &len);
        printf("connfd is %d\n", connfd);
        fflush(stdout);
        if (-1 == connfd) {
          perror("accept fail");
          continue;
        }
        network::setnonblocking(connfd);
        ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
        ev.data.fd = connfd;
        if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev)) {
          perror("epoll_ctl EPOLL_CTL_ADD fail");
          close(connfd);
          continue;
        }
        char buff[INET_ADDRSTRLEN + 1] = {0};
        inet_ntop(AF_INET, &cliaddr.sin_addr, buff, INET_ADDRSTRLEN);
        uint16_t port = ntohs(cliaddr.sin_port);
        printf("connection from %s, port %d\n", buff, port);
      } 
      else if (events[n].events & EPOLLIN) {
        char buffer[MAX_LEN + 1]; 
        int connfd = events[n].data.fd, res;
        if (res = network::read(connfd, buffer)) {
          messageQueue.push(getMessage(buffer));
          printf("received %d bytes\n", res);
        }
      }
      else if (events[n].events & EPOLLOUT) {
        
      }
    }
  }
}

/*
void Broker::work(int client_sockfd) {
  int len;
  char buf[BUFSIZ];
  while ((len = recv(client_sockfd, buf, BUFSIZ, 0)) > 0) {
    buf[len] = '\0';
    messageQueue.push(getMessage(buf));
  }
}*/

std::shared_ptr<Message> Broker::getMessage(char str[]) {
  return std::make_shared<Message>(Message(0, rand() % 10, str, nextMessageID++));
}

void Broker::sendMessage(std::shared_ptr<Message> message) {
  for (std::pair<int, bool> UserID : subscription.getUsers(message->topic)) {
    int clientID = socketTable[UserID.first].socketID;
    if (socketTable[UserID.first].que.empty()) {
      if (send(clientID, message->message.c_str(), message->message.size(), 0) < 0) {
        perror("write error");
        return;
      }
    }
    else {
      socketTable[UserID.first].que.push(message->id);
    }
  }
}

void Broker::resendAll() {
  for(;;) {
    sleep(1);
    for (std::pair<const int, Client>& User : socketTable) {
      if (User.second.que.empty()) continue;
      std::shared_ptr<Message> message = table.getMessage(User.second.que.front());
      if (send(User.second.socketID, message->message.c_str(), message->message.size(), 0) < 0) {
        perror("write error");
        return;
      }
    }
  }
}
