#include "../utility.h"
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <thread>
#include <string>
#include <queue>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

using namespace std;

int cnt = 0;

int working = 1;
char receive[BUFSIZ];
char buf[BUFSIZ];  
char message[BUFSIZ];
int client_sockfd;

unordered_map<int, bool> vis;

queue<pair<string, int>> todo;
mutex mutex_todo;
condition_variable sthtodo;

void subscription() {
  //cout << "subs" << endl;
  while (working) {
    printf("input EXIT to exit, S to subscribe, D to delete subscription:");
    int topic = 904350;
    scanf("%s", buf);
    if (strcmp(buf, "EXIT") == 0) {
      working = 0;
      continue;
    }
    printf("please input the topic: ");
    scanf("%d", &topic);
    printf("topic is %d\n", topic);
    if (buf[0] == 'S')
      newSubscription(message, topic);
    else if (buf[0] == 'D')
      deleteSubscription(message, topic);
    else  
      continue;
    //cout << "sending" << endl;
    socket_send(client_sockfd, message, strlen(message));
  }
}

int read(int sockfd, char* buf) {
  cnt++;
  int res = 0, cc = 0;
  while (1) {
    int ret = recv(sockfd, buf, BUFSIZ, 0);
    if (ret < 0) {
      if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
        //cout << "res, cc = " << res << " " << cc << endl;
        break;
      }
      close(sockfd);
      return 0;
    }
    else if (ret == 0) {
      close(sockfd);
      working = 0;
      return 0;
    }  
    else {
      char toACK[100];
      buf[ret] = '\0';
      res += ret;
      int messageID = 0;
      char* prev = buf;
      char* temp = buf;
      while ((temp = strstr(temp, "\r\n")) != NULL) {
        //*temp = 0;
        while ((temp - buf < ret) && (*temp > '9' || *temp < '0')) temp++;
        if (temp - buf >= ret) break;
        messageID = 0;
        while ('0' <= *temp && *temp <= '9') {
          messageID = messageID * 10 + *temp - '0';
          temp++;
        }
        unique_lock<mutex> lock_todo(mutex_todo);
        todo.push(make_pair(string(prev, strstr(prev, "\r\n") - prev), messageID));
        prev = temp;
        sthtodo.notify_one();
        //printf("received: %s\nid = %d\n", receive, messageID);
        //fflush(stdout);
        // ACK(toACK, messageID);
        // socket_send(client_sockfd, toACK, strlen(toACK));
      }
    }
  }
  return res;
}

void listener() {
  int len;
  struct sockaddr_in remote_addr; 
  memset(&remote_addr, 0, sizeof(remote_addr)); 
  remote_addr.sin_family = AF_INET; 
  remote_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  remote_addr.sin_port = htons(SERVER_PORT); 
  if ((client_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket error");
    exit(EXIT_FAILURE);
  }
  if (connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0) {
    perror("connection error");
    exit(EXIT_FAILURE);
  }
  setnonblocking(client_sockfd);
  
  struct epoll_event ev, events[1000];
  int epollfd = epoll_create1(0);
  if (-1 == epollfd) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }
  ev.events = EPOLLIN | EPOLLET; 
  ev.data.fd = client_sockfd;
  if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sockfd, &ev)) {
    perror("epoll_ctl EPOLL_CTL_ADD fail");
    exit(EXIT_FAILURE);
  }
  while (working) {
    //cout << cnt << endl;
    int nfds = epoll_wait(epollfd, events, 1024, -1);
    if (-1 == nfds) {
      perror("epoll_wait fail");
      exit(EXIT_FAILURE);
    }
    //cout << cnt << endl;
    for (int n = 0; n < nfds; ++n) {
      if (events[n].events & EPOLLIN) {
        int client_sockfd = events[n].data.fd;
        read(client_sockfd, receive);
      }
    }
  }
}

int main() {
  thread t1(subscription);
  t1.detach();

  thread listenThread(listener);
  listenThread.detach();

  while (1) {
    //cout << "dealed " << vis.size() << "Messages" << endl;
    std::unique_lock<std::mutex> lock_todo(mutex_todo);
    while (todo.empty()) 
      sthtodo.wait(lock_todo);
    pair<string, int> message = todo.front();
    todo.pop();
    lock_todo.unlock();
      vis[message.second] = 1;
      if (message.first[0] == 'p')
        cout << message.first << endl;
      char toACK[100];
      ACK(toACK, message.second);
      //cout << "sending" << endl;
      socket_send(client_sockfd, toACK, strlen(toACK));
  }
  return 0;
}