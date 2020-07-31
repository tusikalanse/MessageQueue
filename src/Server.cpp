#include <cstring>
#include <iostream>
#include <thread>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include "Server.h"

// void setnonblocking(int sock) {
// 	int opts;

// 	/*
//     fcntl()函数
//     作用：可以改变已打开的文件性质
//     返回值：fcntl的返回值与第二个参数有关。但是，如果fcntl执行出错，所有的命令都返回-1。
//     参数二：F_GETFL的作用是取得文件描述符sock的文件状态标志，例如：只读打开、只写打开、非阻塞模式等等。其他的文件            状态标志可以查看open()函数。
// 	*/
// 	opts = fcntl(sock, F_GETFL);

// 	if(opts < 0) {
// 		perror("fcntl(sock, GETFL)");
// 		exit(1);
// 	}

// 	opts = opts | O_NONBLOCK; // 给文件状态标志按位或一个非阻塞状态标志，从而让该文件变为非阻塞的。

// 	if(fcntl(sock, F_SETFL, opts) < 0) {
// 		perror("fcntl(sock, SETFL, opts)");
// 		exit(1);
// 	}
// }

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


#define MAX_EVENTS 10
  struct epoll_event ev, events[MAX_EVENTS];
  int listen_sock, conn_sock, nfds, epollfd;

/* Code to set up listening socket, 'listen_sock',
  (socket(), bind(), listen()) omitted */

  epollfd = epoll_create1(0);
  if (epollfd == -1) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }

  ev.events = EPOLLIN;
  ev.data.fd = listen_sock;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
    perror("epoll_ctl: listen_sock");
    exit(EXIT_FAILURE);
  }

  for (;;) {
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      perror("epoll_wait");
      exit(EXIT_FAILURE);
    }

    for (int n = 0; n < nfds; ++n) {
      if (events[n].data.fd == listen_sock) {
        int client_sockfd;
        sockaddr_in client_addr;
        unsigned int sin_size = sizeof(client_addr);
        conn_sock = accept(listen_sock,
                          (struct sockaddr *) &client_addr, &sin_size);
        if (conn_sock == -1) {
          perror("accept");
          exit(EXIT_FAILURE);
        }
        //setnonblocking(conn_sock);
        ev.events = EPOLLIN | EPOLLET;
        ev.data.fd = conn_sock;
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock,
                    &ev) == -1) {
          perror("epoll_ctl: conn_sock");
          exit(EXIT_FAILURE);
        }
      } else {
        //do_use_fd(events[n].data.fd);
      }
    }
    close(epollfd);
  }





  

  // while (1) {
  //   int client_fd = accept(server_sockfd, (struct sockaddr *)&client_addr, &sin_size);
  //   std::thread f(work, client_fd);
  //   f.detach();
  // }
}

void Server::work(int client_sockfd) {
  send(client_sockfd, "Welcome to my MQ!\n", 18, 0);
  int len;
  char buf[BUFSIZ];
  while((len = recv(client_sockfd, buf, BUFSIZ, 0)) > 0) {
    buf[len] = '\0';
    //std::cout << buf << std::endl;
    //fflush(stdout);
    if(send(client_sockfd,buf,len,0) < 0) {
      perror("write error");
      return;
    }
  }
  std::cout << "end " << client_sockfd << std::endl;
}