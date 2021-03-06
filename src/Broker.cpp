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
    priorityNumber(0),
    subscription()
  {}

Broker::Broker(in_port_t _port) : 
    refreshTimeout(1), 
    messageQueue(), 
    table(), 
    port(_port),
    priorityNumber(0),
    subscription()
  {}

Broker::Broker(in_port_t _port, int priority) : 
    refreshTimeout(1), 
    messageQueue(priority), 
    table(), 
    port(_port),
    priorityNumber(priority <= 1 ? 0 : priority),
    subscription()
  {}

Broker::Broker(in_port_t _port, int priority, int _refreshTimeout) : 
    refreshTimeout(_refreshTimeout), 
    messageQueue(priority), 
    table(), 
    port(_port), 
    priorityNumber(priority <= 1 ? 0 : priority),
    subscription() 
  {}

void Broker::run() {
  std::thread listener(&Broker::server, this);
  listener.detach();

  std::thread ticker(&Broker::resendAll, this);
  ticker.detach();

  if (priorityNumber != 0)
    sleep(20); //just for testing

  while (1) {
    std::unique_lock<std::mutex> lock_queue(mutex_queue);
    while (messageQueue.empty()) 
      queue_isnot_empty.wait(lock_queue);
    std::shared_ptr<Message> message = messageQueue.top();
    messageQueue.pop();
    lock_queue.unlock();
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
        std::unique_lock<std::mutex> lock_socketTable(mutex_socketTable);
        socketTable[UserID] = Client(UserID, connfd);
        lock_socketTable.unlock();
        //std::unique_lock<std::mutex> lock_IDTable(mutex_IDTable);
        IDTable[connfd] = UserID;
        //lock_IDTable.unlock();
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
    }
  }
}

void Broker::work(int client_sockfd) {
  //std::unique_lock<std::mutex> lock_IDTable(mutex_IDTable);
  int UserID = IDTable[client_sockfd];
  //lock_IDTable.unlock();
  std::unique_lock<std::mutex> lock_socketTable(mutex_socketTable);
  Client& client = socketTable[UserID];
  lock_socketTable.unlock();
  client.alive = 1;
  read(client);
}

void Broker::sendMessage(std::shared_ptr<Message> message) {
  std::unique_lock<std::mutex> lock_subscription(mutex_subscription);
  int num = subscription.getUsers(message->topic).size();
  std::unique_lock<std::mutex> lock_messageTable(mutex_messageTable);
  table.addMessage(message->id, make_pair(message, num));
  lock_messageTable.unlock();    
  __gnu_pbds::cc_hash_table<int, bool> sub = subscription.getUsers(message->topic);
  lock_subscription.unlock();
  for (std::pair<int, bool> UserID : sub) {
    std::unique_lock<std::mutex> lock_socketTable(mutex_socketTable);
    socketTable[UserID.first].que[message->id] = 1;
    // if (socketTable[UserID.first].que.size() == 1) {
    //std::cout << "Sending " << message->id << " to " << UserID.first << " " << socketTable[UserID.first].alive << std::endl;
    Client& client = socketTable[UserID.first];
    lock_socketTable.unlock();
    if (client.alive == 0) continue;
    send(client, (message->message + "\r\n" + std::to_string(message->id)).c_str(), (message->message + "\r\n" + std::to_string(message->id)).size());
    // }
  }
}

void Broker::resendAll() {
  while (1) {
    sleep(refreshTimeout);
    // std::cout << " ACKTime is " << ACKTime << std::endl;
    // std::cout << " nextMessageID is " << nextMessageID << std::endl;
    // std::cout << " nextUserID is " << nextUserID << std::endl;
    // std::cout << " MQ size is " << messageQueue.size() << std::endl;
    std::unique_lock<std::mutex> lock_socketTable(mutex_socketTable);
    for (std::pair<const int, Client>& User : socketTable) {
      if (User.second.que.empty()) continue;
      if (User.second.alive == 0) continue;
      std::unique_lock<std::mutex> lock_messageTable(mutex_messageTable);
      int cnt = 0;
      //std::cout << "Resending " << User.second.que.size() << " " << ccnt << std::endl;
      for (std::pair<const int, bool>& id: User.second.que) {
        if (++cnt > 10000) break;
        std::shared_ptr<Message> message = table.getMessage(id.first);
        if (message == nullptr) continue;
        //std::cout << "Resending " << message->message << " to " << User.second.UserID << std::endl;
        send(User.second, (message->message + "\r\n" + std::to_string(message->id)).c_str(), (message->message + "\r\n" + std::to_string(message->id)).size());
      }
    }
  }
}

void Broker::removeClient(int sockfd) {
  ///std::unique_lock<std::mutex> lock_IDTable(mutex_IDTable);
  int UserID = IDTable[sockfd];
  IDTable.erase(UserID);
  //lock_IDTable.unlock();
  std::unique_lock<std::mutex> lock_socketTable(mutex_socketTable);
  Client& client = socketTable[UserID];
  lock_socketTable.unlock();
  std::unique_lock<std::mutex> lock_messageTable(mutex_messageTable);
  for (std::pair<const int, bool>& id: client.que) {
    table.ACK(id.first);
    std::shared_ptr<Message> message = table.getMessage(id.first);
  }
  lock_messageTable.unlock();
  std::unique_lock<std::mutex> lock_subscription(mutex_subscription);
  for (std::pair<const int, bool>& sub: client.subscription) {
    subscription.removeSubscription(sub.first, client.UserID);
  }
  lock_subscription.unlock();
  std::unique_lock<std::mutex> lock2_socketTable(mutex_socketTable);
  socketTable.erase(UserID);
  lock2_socketTable.unlock();
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
  //std::cout << "reading " << std::endl;
  int sockfd = client.socketID;
  char* buf = client.buf + client.readIDX;
  int res = 0;
  while (1) {
    buf = client.buf + client.readIDX;
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
      removeClient(sockfd);
      return 0;
    }  
    else {
      buf[ret] = '\0';
      res += ret;
      cnt++;
      HTTPParser(client);
    }
  }
  return res;
}

int Broker::send(Client& client, const char* buf, int len) {
  ccnt++;
  int sockfd = client.socketID;
  int pos = 0, cnt = 0;
  while (pos < len) {
    int ret = ::send(sockfd, buf + pos, len - pos, 0);
    if (-1 == ret) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
        ret = 0;
        if (++cnt > 10) {
          client.alive = 0;
          return 0;
        }
        //perror("error: ");
        //std::cout << "sending to " << client.UserID << std::endl;
        continue;
      }
      return 0;
    }
    else if (0 == ret) {
      return 0;
    }
    //std::cout << ret << std::endl;
    cnt = 0;
    pos += ret;
  }
  return pos;
}

void Broker::HTTPParser(Client& client) {
  int n = strlen(client.buf), IDX = 0;
  while (IDX < n) {
    while (IDX < n && (client.buf[IDX] == '\n' || client.buf[IDX] == '\r')) IDX++;
    if (client.buf[IDX] == 'G') {
      const char* temp = strstr(client.buf + IDX, "\r\n\r\n");
      if (temp == NULL) {
        strncpy(client.buf, client.buf + IDX, n - IDX);
        client.buf[n - IDX] = '\0';
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
      if (NULL == strstr(temp, "\r\n\r\n")) {
        strncpy(client.buf, client.buf + IDX, n - IDX);
        client.readIDX = n - IDX;
        return;
      }
      while (ch < '0' || ch > '9') ch = *++temp;
      while (ch >= '0' && ch <= '9') {
        length = length * 10 + ch - '0';
        ch = *++temp;
      }
      temp = strstr(temp, "\r\n\r\n");
      if (temp + 4 + length > client.buf + n) {
        strncpy(client.buf, client.buf + IDX, n - IDX);
        client.readIDX = n - IDX;
        return;
      }
      if (client.buf[IDX] == 'D') 
        dealDelete(client, client.buf + IDX, temp + 4, length);
      else if (client.buf[IDX + 1] == 'O')
        dealPost(client, client.buf + IDX, temp + 4, length);
      else if (client.buf[IDX + 1] == 'U')
        dealPut(client, client.buf + IDX, temp + 4, length);
      else if (IDX != n) {
        // std::cerr << "Bad Request" << std::endl;
        // std::cout << client.buf << std::endl;
        // std::cout << "IDX, n = " << IDX << " " << n << std::endl;
        // std::cout << "near IDX = " << client.buf[IDX] << client.buf[IDX + 1] << client.buf[IDX + 2] << client.buf[IDX + 3] << client.buf[IDX + 4] << client.buf[IDX + 5] << client.buf[IDX + 6] << client.buf[IDX + 7] << client.buf[IDX + 8] << client.buf[IDX + 9] << client.buf[IDX + 10] << std::endl;
      }
      IDX = temp - client.buf + length + 4;
    }
  }
  if (IDX != n) std::cerr << "error IDX != n" << std::endl;
  client.readIDX = 0;
}

int Broker::dealGet(const char* buf, int len) {
  //todo
}


int Broker::dealPost(Client& client, const char* buf, const char* body, int len) {
  //url: .../subscription
  //body: topic=$topic
  //url: .../message
  //body: message=$message&topic=$topic(&priority=$priority)
  const char* temp = strchr(buf, '/');
  if (strstr(temp, "subscription") == temp + 1) {
    if (strstr(body, "topic=") != body) return 400;
    temp = strchr(body, '=') + 1;
    int id = atoi(temp);
    std::unique_lock<std::mutex> lock_subscription(mutex_subscription);
    subscription.addSubscription(id, client.UserID);
    // std::cout << client.UserID << " subscribed " << id << std::endl;
    client.subscription[id] = 1;
    return 200;
  }
  else if (strstr(temp, "message") == temp + 1) {
    if (strstr(body, "message=") != body) return 400;
    if (strstr(body, "&topic=") == NULL) return 400;
    if (priorityNumber != 0) {
      if (strstr(body, "&priority=") == NULL) return 400;
      temp = strstr(body, "&priority=") + 10;
      int priority = atoi(temp);
      if (priority <= 1) priority = 1;
      if (priority > priorityNumber) return 400;
      temp = strstr(body, "&topic=") + 7;
      int topic = atoi(temp);
      std::string str(body + 8, temp - body - 15);
      std::unique_lock<std::mutex> lock_queue(mutex_queue);
      //std::cout << "priority = " << priority << std::endl;
      messageQueue.push(std::make_shared<PriorityMessage>(PriorityMessage(client.UserID, topic, str, nextMessageID++, priority)));
    }
    else {
      temp = strstr(body, "&topic=") + 7;
      int topic = atoi(temp);
      std::string str(body + 8, temp - body - 15);
      std::unique_lock<std::mutex> lock_queue(mutex_queue);
      messageQueue.push(std::make_shared<Message>(Message(client.UserID, topic, str, nextMessageID++)));
    }
    queue_isnot_empty.notify_one();
    return 200;
  }
  else
    return 400;
}

int Broker::dealPut(Client& client, const char* buf, const char* body, int len) {
  //url: .../ACK
  //body: messageID=$ID
  const char* temp = strchr(buf, '/');
  if (strstr(temp, "ACK") != temp + 1) return 400;
  if (strstr(body, "messageID=") != body) return 400;
  temp = strchr(body, '=') + 1;
  int id = atoi(temp);
  //std::cout << client.UserID << " ack " << id << std::endl;
  std::unique_lock<std::mutex> lock_socketTable(mutex_socketTable);
  if (client.que.find(id) != client.que.end()) {
    client.que.erase(id);
    lock_socketTable.unlock();
    std::unique_lock<std::mutex> lock_messageTable(mutex_messageTable);
    ACKTime++;
    table.ACK(id);
    //std::cout << client.UserID << " ACK " << id << std::endl;
    lock_messageTable.unlock();
    // if (!client.que.empty()) {
    //   std::unique_lock<std::mutex> lock_messageTable(mutex_messageTable);
    //   std::shared_ptr<Message> message = table.getMessage(client.que.front());
    //   lock_messageTable.unlock();
    //   send(client, (message->message + "\r\n" + std::to_string(message->id)).c_str(), message->message.size() + 2 + std::to_string(message->id).size());
    // }
  }
  // else {
  //   std::cout << client.UserID << " ack " << id << " need " << client.que.front() << std::endl; 
  // }
  return 200;
}

int Broker::dealDelete(Client& client, const char* buf, const char* body, int len) {
  //url: .../subscription
  //body: topic=$topic
  const char* temp = strchr(buf, '/');
  if (strstr(temp, "subscription") != temp + 1) return 400;
  if (strstr(body, "topic=") != body) return 400;
  temp = strchr(body, '=') + 1;
  int id = atoi(temp);
  std::unique_lock<std::mutex> lock_subscription(mutex_subscription);
  subscription.removeSubscription(id, client.UserID);
  client.subscription.erase(id);
  return 200;
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
