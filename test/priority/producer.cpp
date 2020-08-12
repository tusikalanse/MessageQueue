#include "../utility.h"
#include <iostream>
#include <cstring>

using namespace std;

int pow10[] = {1, 10, 100, 1000, 10000, 100000, 1000000};

int main() {
  int client_sockfd;
  int len;
  struct sockaddr_in remote_addr; 
  char buf[BUFSIZ];  
  char message[BUFSIZ];
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
  for (int i = 1; i <= 100000; ++i) {
    int len = rand() % 10 + 10;
    for (int j = 0; j < len; ++j)
      buf[j] = 'a' + rand() % 26;
    buf[len] = '\0';
    int topic = 1;
    newMessage(message, buf, topic, 1);
    //cout << "sending" << endl;
    //send(client_sockfd, message, strlen(message), 0);
    socket_send(client_sockfd, message, strlen(message));
  }
  for (int i = 100; i >= 1; --i) {
    int len = 1 + getLen(i);
    buf[0] = 'p';
    for (int j = 1; j < len; ++j)
      buf[j] = '0' + (i % pow10[len - j]) / pow10[len - j - 1];
    buf[len] = '\0';
    int topic = 1;
    newMessage(message, buf, topic, i);
    //cout << "sending" << endl;
    //send(client_sockfd, message, strlen(message), 0);
    socket_send(client_sockfd, message, strlen(message));
  }
  close(client_sockfd);
  return 0;
}