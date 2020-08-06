#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/epoll.h>
#include "Broker.h"
#include "network.h"

Broker::Broker() : 
    refreshTimeout(1), 
    messageQueue(), 
    table(), 
    port(8000),
    subscription()
  {}

Broker::Broker(in_port_t _port) : 
    refreshTimeout(1), 
    messageQueue(), 
    table(), 
    port(_port),
    subscription()
  {}

Broker::Broker(in_port_t _port, int priority) : 
    refreshTimeout(1), 
    messageQueue(priority), 
    table(), 
    port(_port),
    subscription()
  {}

Broker::Broker(in_port_t _port, int priority, int _refreshTimeout) : 
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
    std::unique_lock<std::mutex> lock(mutex_queue);
    while (messageQueue.empty()) 
      queue_isnot_empty.wait(lock);
    // if (messageQueue.empty()) {
    //   // sleep(1);
    //   // std::cout << "empty message queue" << std::endl;
    //   // std::cout << "nextMessageID is " << nextMessageID << std::endl;
    //   continue;
    // }
    std::cout << "message queue size is " << messageQueue.size() << std::endl;
    std::cout << "nextMessageID is " << nextMessageID << std::endl;
    std::shared_ptr<Message> message = messageQueue.top();
    messageQueue.pop();
    ++out;
    lock.unlock();
    // TODO : ACK message
    std::cout << "out cnt " << out << std::endl;
    sendMessage(message);
  }

  std::cout << "end main thread" << std::endl;

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

  int opt = 1;
  setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));

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
    std::cout << "epoll waiting" << std::endl;
    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (-1 == nfds) {
      perror("epoll_wait fail");
      exit(EXIT_FAILURE);
    }
    for (int n = 0; n < nfds; ++n) {
      if (events[n].data.fd == server_sockfd) {
        std::cout << "new connection" << std::endl;
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
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = connfd;
        if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev)) {
          perror("epoll_ctl EPOLL_CTL_ADD fail");
          close(connfd);
          continue;
        }
        int UserID = nextUserID++;
        std::unique_lock<std::mutex> lock(mutex_socketTable);
        socketTable[UserID] = Client(UserID, connfd);
        lock.unlock();
        //test
        printf("new UserID = %d\n", UserID); 
        if (UserID >= 1) {
          printf("%d subscribed: ", UserID);
          for (int i = 0; i < 10; ++i) {
            if (rand() % 3 == 1) {
              printf(" %d", i);
              std::unique_lock<std::mutex> lock(mutex_subscription);
              subscription.addSubscription(i , UserID);
            }
          }
          puts("");
        }
        char buff[INET_ADDRSTRLEN + 1] = {0};
        inet_ntop(AF_INET, &cliaddr.sin_addr, buff, INET_ADDRSTRLEN);
        uint16_t port = ntohs(cliaddr.sin_port);
        printf("connection from %s, port %d\n", buff, port);
      } 
      else if (events[n].events & EPOLLIN) {
        int client_sockfd = events[n].data.fd;
        std::thread reader(&Broker::work, this, client_sockfd);
        reader.join();
      }
      // else if (events[n].events & EPOLLOUT) {
        
      // }
    }
  }
}

void Broker::work(int client_sockfd) {
  std::cout << "reading" << std::endl;
  char buffer[MAX_LEN + 1]; 
  int res;
  std::cout << "readdding" << std::endl;
  if (res = network::read(client_sockfd, buffer)) {
    std::cout << "inner reading" << std::endl;
    std::shared_ptr<Message> message = getMessage(buffer);
    std::unique_lock<std::mutex> lock(mutex_queue);    
    messageQueue.push(message);
    lock.unlock();
    queue_isnot_empty.notify_one();
    //printf("received %d bytes\n", res);
  }
  std::cout << "read end" << std::endl;
}

std::shared_ptr<Message> Broker::getMessage(char str[]) {
  return std::make_shared<Message>(Message(0, rand() % 10, str, nextMessageID++));
}

void Broker::sendMessage(std::shared_ptr<Message> message) {
  printf("sending message topic is %d\n", message->topic);
  fflush(stdout);
  std::unique_lock<std::mutex> lock(mutex_socketTable);
  std::unique_lock<std::mutex> locker(mutex_subscription);
  for (std::pair<int, bool> UserID : subscription.getUsers(message->topic)) {
    int clientID = socketTable[UserID.first].socketID;
    if (socketTable[UserID.first].que.empty()) {
      int res;
      network::send(clientID, message->message.c_str(), message->message.size());
    }
    else {
      socketTable[UserID.first].que.push(message->id);
    }
  }
}

void Broker::resendAll() {
  while (1) {
    sleep(1);
    std::unique_lock<std::mutex> lock(mutex_socketTable);
    for (std::pair<const int, Client>& User : socketTable) {
      if (User.second.que.empty()) continue;
      std::cout << "Resending" << std::endl;
      std::unique_lock<std::mutex> lock(mutex_messageTable);
      std::shared_ptr<Message> message = table.getMessage(User.second.que.front());
      lock.unlock();
      if (network::send(User.second.socketID, message->message.c_str(), message->message.size()) < 0) {
        perror("write error");
        return;
      }
    }
  }
}
