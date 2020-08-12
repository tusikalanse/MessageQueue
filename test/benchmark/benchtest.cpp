#include "../utility.h"
#include <algorithm>
#include <iostream>
#include <cstring>
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
#include <vector>
#include <cassert>

using namespace std;

int producerCount, messageNumberPerProducer, messageLenMin, messageLenMax, topicNumber, consumerNumber,subscriptionsPerConsumer;

int count_finished = 0;
mutex mutex_finished;
condition_variable all_finished;

namespace producer {

  int main(int messageNumberPerProducer, int messageLenMin, int messageLenMax, int topicNumber) {
    cout  << "new consumer" << endl;
    int client_sockfd;
    cout  << "new consumer" << endl;
    int len;
    struct sockaddr_in remote_addr; 
    char buf[BUFSIZ];  
    char message[BUFSIZ];
    memset(&remote_addr, 0, sizeof(remote_addr)); 
    remote_addr.sin_family = AF_INET; 
    remote_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    remote_addr.sin_port = htons(SERVER_PORT); 
    int messageNumberPerTopic = messageNumberPerProducer / topicNumber;
    int messageRandLen = messageLenMax - messageLenMin;
    cout  << "new consumer" << endl;

    if ((client_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
      perror("socket error");
      return 1;
    }

    if (connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0) {
      perror("connection error");
      return 1;
    }
    cout  << "new consumer" << endl;

    setnonblocking(client_sockfd);
    for (int i = 0; i < messageNumberPerProducer; ++i) {
      int len = rand() % messageRandLen + messageLenMin;
      for (int j = 0; j < len; ++j)
        buf[j] = 'a' + rand() % 26;
      buf[len] = '\0';
      int topic = i / messageNumberPerTopic;
      newMessage(message, buf, topic);
      socket_send(client_sockfd, message, strlen(message));
    }
    close(client_sockfd);
    unique_lock<mutex> lock_finished;
    if (++count_finished == producerCount + consumerNumber) 
      all_finished.notify_one();
    return 0;
  }

}

namespace consumer {

  int cnt = 0;

  int working = 1;
  char receive[BUFSIZ];
  char buf[BUFSIZ];  
  char message[BUFSIZ];
  int client_sockfd;

  unordered_map<int, int> vis;

  queue<pair<string, int>> todo;
  mutex mutex_todo;
  condition_variable sthtodo;

  int read(int sockfd, char* buf) {
    cnt++;
    int res = 0, cc = 0;
    while (1) {
      int ret = recv(sockfd, buf, BUFSIZ, 0);
      if (ret < 0) {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
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
        }
      }
    }
    return res;
  }

  void listener(vector<int>& sub) {
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

    for (int topic = 0; topic < sub.size(); ++topic) {
      if (sub[topic] == 0) continue;
      newSubscription(message, topic);
      socket_send(client_sockfd, message, strlen(message));
    }

    while (working) {
      //cout << cnt << endl;
      int nfds = epoll_wait(epollfd, events, 1024, -1);
      if (-1 == nfds) {
        perror("epoll_wait fail");
        exit(EXIT_FAILURE);
      }
      for (int n = 0; n < nfds; ++n) {
        if (events[n].events & EPOLLIN) {
          int client_sockfd = events[n].data.fd;
          read(client_sockfd, receive);
        }
      }
    }
  }

  int main(int topicNumber, int subscriptionsPerConsumer, int consumerMessage) {
    vector<int> sub;
    for (int i = 0; i < subscriptionsPerConsumer; ++i) sub.push_back(1);
    for (int i = subscriptionsPerConsumer; i < topicNumber; ++i) sub.push_back(0);
    random_shuffle(sub.begin(), sub.end());
    thread listenThread(listener, ref(sub));
    listenThread.detach();

    while (1) {
      std::unique_lock<std::mutex> lock_todo(mutex_todo);
      while (todo.empty()) 
        sthtodo.wait(lock_todo);
      pair<string, int> message = todo.front();
      todo.pop();
      lock_todo.unlock();
      char toACK[100];
      ACK(toACK, message.second);
      socket_send(client_sockfd, toACK, strlen(toACK));
      vis[message.second] = 0;
      if (vis.size() == consumerMessage) {
        working = 0;
        unique_lock<mutex> lock_finished;
        if (++count_finished == producerCount + consumerNumber) 
          all_finished.notify_one();
        break;
      }
    }
    return 0;
  }
}


//producerCount, messageNumberPerProducer, messageLenMin, messageLenMax, topicNumber, consumerNumber, subscriptionsPerConsumer
int main(int argc, char **argv) {
  assert(argc == 8);
  producerCount = atoi(argv[1]);
  messageNumberPerProducer = atoi(argv[2]);
  messageLenMin = atoi(argv[3]);
  messageLenMax = atoi(argv[4]);
  topicNumber = atoi(argv[5]);
  consumerNumber = atoi(argv[6]);
  subscriptionsPerConsumer = atoi(argv[7]);
  int consumerMessage = topicNumber / subscriptionsPerConsumer * producerCount * messageNumberPerProducer;
  for (int i = 0; i < consumerNumber; ++i) {
    thread t1(consumer::main, topicNumber, subscriptionsPerConsumer, consumerMessage);
    t1.detach();
  }

  for (int i = 0; i < producerCount; ++i) {
    thread t1(producer::main, messageNumberPerProducer, messageLenMin, messageLenMax, topicNumber);
    t1.detach();
  }

  unique_lock<mutex> lock_finished;

  while (count_finished != producerCount + consumerNumber) 
    all_finished.wait(lock_finished);

  return 0;
}