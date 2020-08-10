#include "../utility.h"
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <thread>

using namespace std;

int working = 1;
char receive[BUFSIZ];
char buf[BUFSIZ];  
char message[BUFSIZ];
int client_sockfd;

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
  int res = 0;
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
      char* temp = strstr(receive, "\r\n");
      while ((temp = strstr(receive, "\r\n")) != NULL) {
        *temp = 0;
        while (*temp > '9' || *temp < '0') temp++;
        while ('0' <= *temp && *temp <= '9') {
          messageID = messageID * 10 + *temp - '0';
          temp++;
        }
        printf("received: %s\nid = %d\n", receive, messageID);
        fflush(stdout);
        ACK(toACK, messageID);
        socket_send(client_sockfd, toACK, strlen(toACK));
      }
    }
  }
  return res;
}


int main() {
  int len;
  struct sockaddr_in remote_addr; 
  memset(&remote_addr, 0, sizeof(remote_addr)); 
  remote_addr.sin_family = AF_INET; 
  remote_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  remote_addr.sin_port = htons(SERVER_PORT); 
  if ((client_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket error");
    return 1;
  }
  if (connect(client_sockfd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) < 0) {
    perror("connection error");
      return 1;
  }
  setnonblocking(client_sockfd);
  
  struct epoll_event ev, events[1000];
  int epollfd = epoll_create1(0);
  if (-1 == epollfd) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }
  ev.events = EPOLLIN; 
  ev.data.fd = client_sockfd;
  if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, client_sockfd, &ev)) {
    perror("epoll_ctl EPOLL_CTL_ADD fail");
    exit(EXIT_FAILURE);
  }

  thread t1(subscription);
  //cout << "dd"<< endl;
  t1.detach();

  while (working) {
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
  return 0;
}