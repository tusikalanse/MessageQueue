#include <cstring>
#include <iostream>
#include <thread>
#include "Server.h"

Server::Server() : port(8000) {}

Server::Server(int _port) : port(_port) {}

int Server::run() {
  int server_sockfd;
  sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr.sin_port = htons(port);
  
  if ((server_sockfd = socket(PF_INET,SOCK_STREAM,0)) < 0) {  
    perror("socket error");
    return 1;
  }

  if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind error");
    return 1;
  }

  if (listen(server_sockfd, 128) < 0) {
    perror("listen error");
    return 1;
  }

  int client_sockfd;
  sockaddr_in client_addr;
  unsigned int sin_size = sizeof(client_addr);

  while (1) {
    int client_fd = accept(server_sockfd, (struct sockaddr *)&client_addr, &sin_size);
    std::thread f(work, client_fd);
    f.detach();
  }
}

void Server::work(int client_sockfd) {
  send(client_sockfd, "Welcome to my MQ!\n", 18, 0);
  int len;
  char buf[BUFSIZ];
  while((len = recv(client_sockfd, buf, BUFSIZ, 0)) > 0) {
    buf[len] = '\0';
    std::cout << buf << std::endl;
    fflush(stdout);
    if(send(client_sockfd,buf,len,0) < 0) {
      perror("write error");
      return;
    }
  }
  std::cout << "end " << client_sockfd << std::endl;
}