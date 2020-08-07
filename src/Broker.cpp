#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <sys/epoll.h>
#include "Broker.h"

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
        setnonblocking(connfd);
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
        std::unique_lock<std::mutex> locker(mutex_IDTable);
        IDTable[connfd] = UserID;
        locker.unlock();
        //test
        printf("new UserID = %d\n", UserID); 
        if (UserID >= 1) {
          printf("%d subscribed: ", UserID);
          for (int i = 0; i < 10; ++i) {
            if (rand() % 3 == 1) {
              printf(" %d", i);
              std::unique_lock<std::mutex> lock(mutex_subscription);
              //subscription.addSubscription(i , UserID);
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
  std::unique_lock<std::mutex> lock(mutex_IDTable);
  int UserID = IDTable[client_sockfd];
  lock.unlock();
  std::unique_lock<std::mutex> locker(mutex_socketTable);
  Client& client = socketTable[UserID];
  locker.unlock();
  if (res = read(client)) {
    std::cout << "inner reading" << std::endl;
    std::shared_ptr<Message> message = getMessage(buffer);
    std::unique_lock<std::mutex> lock(mutex_queue);    
    messageQueue.push(message);
    lock.unlock();
    queue_isnot_empty.notify_one();
    //printf("received %d bytes\n", res);
    //::send(client_sockfd, "HTTP/1.1 200 OK\r\nContent-Length: 25\r\n\r\n<html>Hello World!</html>", 64, 0);
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
      std::unique_lock<std::mutex> lock(mutex_IDTable);
      int UserID = IDTable[clientID];
      lock.unlock();
      std::unique_lock<std::mutex> locker(mutex_socketTable);
      Client& client = socketTable[UserID];
      locker.unlock();
      send(client, message->message.c_str(), message->message.size());
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
      if (send(User.second, message->message.c_str(), message->message.size()) < 0) {
        perror("write error");
        return;
      }
    }
  }
}

void Broker::setnonblocking(int sockfd) {
  int flag = fcntl(sockfd, F_GETFL, 0);
  if (flag < 0) {
    perror("fcntl F_GETFL fail");
    return;
  }
  if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0) {
    perror("fcntl F_SETFL fail");
  }
}

int Broker::read(Client& client) {
  int sockfd = client.socketID;
  char* buf = client.buf + client.readIDX;
  int res = 0;
  while (1) {
    int ret = recv(sockfd, buf, BUFSIZ - client.readIDX - 10, 0);
    if (ret < 0) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
        break;
      }
      close(sockfd);
      return 0;
    }
    else if (ret == 0) {
      close(sockfd);
      return 0;
    }  
    else {
      buf[ret] = '\0';
      res += ret;
      //HTTPParser(client);
      //printf("%d %d: %s\n", ret, strlen(buf), buf);
      //fflush(stdout);
    }
  }
  return res;
}

int Broker::send(Client& client, const char* buf, int len) {
  int sockfd = client.socketID;
  int pos = 0;
  while (pos < len) {
    int ret = ::send(sockfd, buf + pos, len - pos, 0);
    if (-1 == ret) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
        ret = 0;
        continue;
      }
      return 0;
    }
    pos += ret;
  }
  return pos;
}

void Broker::HTTPParser(Client& client) {
  int n = strlen(client.buf), IDX = 0;
  if (findIndex(client.buf) != 0) {
    std::cerr << "error" << std::endl;
  }
  while (IDX < n) {
    if (client.buf[IDX] == 'G') {
      const char* temp = strstr(client.buf + IDX, "\r\n\r\n");
      if (temp == NULL) {
        strncpy(client.buf, client.buf + IDX, n - IDX);
        client.readIDX = n - IDX;
        return;
      }
      dealGet(client.buf + IDX, temp - client.buf + 4 - IDX);
      IDX = temp - client.buf + 4;
    }
    else {
      const char* temp = strstr(client.buf + IDX, "Content-Length:");
      if (temp == NULL) {
        strncpy(client.buf, client.buf + IDX, n - IDX);
        client.readIDX = n - IDX;
        return;
      }
      int length = 0;
      char ch = *temp;
      while (ch < '0' || ch > '9') ch = *++temp;
      while (ch >= '0' && ch <= '9') {
        length = length * 10 + ch - '0';
        ch = *++temp;
      }
      temp = strstr(temp, "\r\n\r\n");
      if (temp == NULL || temp + 4 + length > client.buf + n) {
        strncpy(client.buf, client.buf + IDX, n - IDX);
        client.readIDX = n - IDX;
        return;
      }
      if (client.buf[IDX] == 'D') 
        dealDelelte(client.buf + IDX, temp + 4, length);
      else if (client.buf[IDX + 1] == 'O')
        dealPost(client.buf + IDX, temp + 4, length);
      else if (client.buf[IDX + 1] == 'U')
        dealPut(client.UserID, client.buf + IDX, temp + 4, length);
      else
        if (IDX != n) std::cerr << "Bad Request" << std::endl;
      IDX = temp - client.buf + length + 4;
    }
  }
  if (IDX != n) std::cerr << "error IDX != n" << std::endl;
  client.readIDX = 0;
}

int Broker::dealGet(const char* buf, int len) {
  //todo
}


int Broker::dealPost(const char* buf, const char* body, int len) {
  //todo
}

int Broker::dealPut(int UserID, const char* buf, const char* body, int len) {
  //url: .../ACK
  //body: messageID=$ID
  const char* temp = strchr(buf, ' ');
  if (strstr(temp, "ACK") != temp + 1) return 400;
  if (strstr(body, "messageID=") != body) return 400;
  temp = strchr(body, '=') + 1;
  int id = atoi(temp);
  


}

int Broker::dealDelelte(const char* buf, const char* body, int len) {
  //todo
}

int Broker::findIndex(const char* buf) {
  const char* result;
  int res = 1e9;
  result = strstr(buf, "GET");
  if (result != NULL && result - buf < res) {
    res = result - buf;
  }
  result = strstr(buf, "POST");
  if (result != NULL && result - buf < res) {
    res = result - buf;
  }
  result = strstr(buf, "PUT");
  if (result != NULL && result - buf < res) {
    res = result - buf;
  }
  result = strstr(buf, "DELETE");
  if (result != NULL && result - buf < res) {
    res = result - buf;
  }
  if (res == 1e9) return -1;
  return res;
}