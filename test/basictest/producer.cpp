#include "../utility.h"
#include <iostream>
#include <cstring>

using namespace std;

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
  while (1) {
    printf("input EXIT to exit:");
    int topic;
    scanf("%s", buf);
    if (strcmp(buf, "EXIT") == 0) break;
    scanf("%d", &topic);
    newMessage(message, buf, topic);
    cout << "sending" << endl;
    socket_send(client_sockfd, message, strlen(message));
  }
  close(client_sockfd);
  return 0;
}