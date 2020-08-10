#include "../utility.h"
#include <iostream>
#include <cstring>
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
  //setnonblocking(client_sockfd);
  
  thread t1(subscription);
  //cout << "dd"<< endl;
  t1.detach();

  working = 1;

  char toACK[100];
  while (working) {
    int messageID = 0;
    bzero(&receive, BUFSIZ);
    cout << "waiting" << endl;
    int ret = recv(client_sockfd, receive, BUFSIZ, 0);
    cout << "ret = " << ret << endl;
    if (ret == 0) {
      working = 0;
      printf("Server closed\n");
      continue;
    }
    char* temp = strstr(receive, "\r\n");
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
  close(client_sockfd);
  return 0;
}