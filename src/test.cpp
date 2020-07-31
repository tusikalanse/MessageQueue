// #include <bits/extc++.h>
// #include <bits/stdc++.h>
// using namespace std;
// #pragma GCC optimize("O0")

// const int N = 1e8;

// struct foo {
//   foo() {}
//   foo(int _a) : a(_a) {}
//   int a;
// };

// struct FooComparator {
//   auto operator()(const std::shared_ptr<foo>& lhs, const std::shared_ptr<foo>& rhs) const 
//     -> bool
//   { return lhs->a > rhs->a; }
// };

// int a[N];
// int main() {
//   auto cmp = [] (const std::shared_ptr<foo>& lhs, const std::shared_ptr<foo>& rhs) {return lhs->a > rhs->a;};
//   std::priority_queue<std::shared_ptr<foo>, vector<std::shared_ptr<foo>>, FooComparator> pq;
//   std::shared_ptr<foo> x[100];
//   for (int i = 0; i < 100; ++i) {
//     x[i] = std::make_shared<foo>(rand() % 10);
//     pq.push(x[i]);
//   }
//   for (int i = 0; i < 100; ++i) {
//     std::shared_ptr<foo> x = pq.top();
//     pq.pop();
//     cout << x->a << endl;
//   }
//   return 0;
// }


// #include <bits/stdc++.h>
// using namespace std;

// struct foo {
//   virtual int hi() {cout << "dsds" << endl;}
// };

// struct bar : public foo {
//   int hi() {cout << "ddd" << endl;}
//   int hh() {cout << "ddddddd" << endl;}
// };

// int main() {
//   std::shared_ptr<foo> x = std::make_shared<bar>(bar());
//   x->hi();
//   std::shared_ptr<bar> xx = std::dynamic_pointer_cast<bar>(x);
//   xx->hh();

//   return 0;
// }

// #include <bits/stdc++.h>
// #include "MyMessageQueue.h"
// #include "PriorityMessage.h"
// using namespace std;

// int main() {
//   MyMessageQueue MQ(100);
//   for (int i = 1; i <= 100; ++i) {
//     MQ.push(std::shared_ptr<Message>(new PriorityMessage(1, 2, to_string(rand()), 1, rand() % 4)));
//   }
//   for (int i = 1; i <= 100; ++i) {
//     shared_ptr<PriorityMessage> M = std::dynamic_pointer_cast<PriorityMessage>(MQ.top());
//     MQ.pop();
//     cout << MQ.size() << " " << boolalpha << MQ.empty() << " ";
//     cout << M->message << " " << M->priority << endl;
//   }
//   return 0;
// }

// #include <bits/extc++.h>
// #include <bits/stdc++.h>
// using namespace std;
// using namespace __gnu_pbds;

// int main() {
//   gp_hash_table<int, int> h;
//   h[1] = 1;
//   h[2] = 3;
//   h[4] = 5; 
//   h[5] = 6;
//   for (gp_hash_table<int, int>::iterator it = h.begin(); it != h.end(); ) {
//     cout << it->first << ": " << it->second << endl;
//     if (it->second == 1) {
//       h.erase(it++->first);
//     }
//     else
//     {
//         it++;
//     }
    
//   }

//   for (gp_hash_table<int, int>::iterator it = h.begin(); it != h.end(); ++it) {
//     cout << it->first << ": " << it->second << endl;
//   }
  

//   return 0;
// }

// #include <bits/stdc++.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// using namespace std;

// int main(int argc, char *argv[])
// {
//     int server_sockfd;//服务器端套接字
//     int client_sockfd;//客户端套接字
//     int len;
//     struct sockaddr_in my_addr;   //服务器网络地址结构体
//     struct sockaddr_in remote_addr; //客户端网络地址结构体
//     unsigned int sin_size;
//     char buf[BUFSIZ];  //数据传送的缓冲区
//     memset(&my_addr,0,sizeof(my_addr)); //数据初始化--清零
//     my_addr.sin_family=AF_INET; //设置为IP通信
//     my_addr.sin_addr.s_addr=INADDR_ANY;//服务器IP地址--允许连接到所有本地地址上
//     my_addr.sin_port=htons(8000); //服务器端口号
    
//     /*创建服务器端套接字--IPv4协议，面向连接通信，TCP协议*/
//     if((server_sockfd=socket(PF_INET,SOCK_STREAM,0))<0)
//     {  
//         perror("socket error");
//         return 1;
//     }
 
 
//     /*将套接字绑定到服务器的网络地址上*/
//     if(bind(server_sockfd,(struct sockaddr *)&my_addr,sizeof(struct sockaddr))<0)
//     {
//         perror("bind error");
//         return 1;
//     }
    
//     /*监听连接请求--监听队列长度为5*/
//     if(listen(server_sockfd,5)<0)
//     {
//         perror("listen error");
//         return 1;
//     };
    
//     sin_size=sizeof(struct sockaddr_in);
    
//     /*等待客户端连接请求到达*/
//     if((client_sockfd=accept(server_sockfd,(struct sockaddr *)&remote_addr,&sin_size))<0)
//     {
//         perror("accept error");
//         return 1;
//     }
//     printf("accept client %s\n",inet_ntoa(remote_addr.sin_addr));
//     len=send(client_sockfd,"Welcome to my server\n",21,0);//发送欢迎信息
    
//     /*接收客户端的数据并将其发送给客户端--recv返回接收到的字节数，send返回发送的字节数*/
//     while((len=recv(client_sockfd,buf,BUFSIZ,0))>0)
//     {
//         buf[len]='\0';
//         printf("%s\n",buf);
//         fflush(stdout);
//         if(send(client_sockfd,buf,len,0)<0)
//         {
//             perror("write error");
//             return 1;
//         }
//     }
 
 

    
//     return 0;
// }

// #include <iostream>
// #include <cstdlib>
// #include "Message.h"
// #include "MyMessageQueue.h"
// #include "Subscription.h"

// const int N = 1e6;

// int main() {
//   MyMessageQueue mq();
//   for (int i = 0; i < N; ++i) {
    
//   }
//   std::cout << "FDSFD" << std::endl;

//   return 0;
// }


// #include <bits/stdc++.h>
// #include "Broker.h"
// #include "Server.h"

// int main() {
//   Server server(8000);
//   server.run();

//   return 0;
// }


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define MAX_EVENTS 1000
#define MAX_LEN 1024

void setnonblocking(int sockfd) {
    int flag = fcntl(sockfd, F_GETFL, 0);
    if (flag < 0) {
        perror("fcntl F_GETFL fail");
        return;
    }
    if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0) {
        perror("fcntl F_SETFL fail");
    }
}

int main() {
  int listenfd;
  struct sockaddr_in servaddr;
  short port = 8000;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  setnonblocking(listenfd); 
  int res = bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));
  if (0 == res)
    printf("server bind success, 0.0.0.0:%d\n", port);
  else {
    perror("bind fail");
    exit(EXIT_FAILURE);
  }
  res = listen(listenfd, 128);
  if (0 == res)
    printf("server listen success\n");
  else {
    perror("listen fail");
    exit(EXIT_FAILURE);
  }
  struct epoll_event ev, events[MAX_EVENTS];
  int epollfd = epoll_create1(0);
  if (-1 == epollfd) {
    perror("epoll_create1");
    exit(EXIT_FAILURE);
  }
  ev.events = EPOLLIN; 
  ev.data.fd = listenfd;
  if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev)) {
    perror("epoll_ctl EPOLL_CTL_ADD fail");
    exit(EXIT_FAILURE);
  }
  for (;;) {
    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (-1 == nfds) {
      perror("epoll_wait fail");
      exit(EXIT_FAILURE);
    }
    for (int n = 0; n < nfds; ++n) {
      if (events[n].data.fd == listenfd) {
        if (!(events[n].events & EPOLLIN)) continue;
        struct sockaddr_in cliaddr;
        socklen_t len = sizeof(cliaddr);
        int connfd = accept(listenfd, (sockaddr *)&cliaddr, &len);
        printf("connfd is %d\n", connfd);
        fflush(stdout);
        if (-1 == connfd) {
          perror("accept fail");
          continue;
        }
        setnonblocking(connfd);
        ev.events = EPOLLIN | EPOLLET | EPOLLOUT;
        ev.data.fd = connfd;
        if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &ev)) {
            perror("epoll_ctl EPOLL_CTL_ADD fail");
            close(connfd);
            continue;
        }
        char buff[INET_ADDRSTRLEN + 1] = {0};
        inet_ntop(AF_INET, &cliaddr.sin_addr, buff, INET_ADDRSTRLEN);
        uint16_t port = ntohs(cliaddr.sin_port);
        printf("connection from %s, port %d\n", buff, port);
      } 
      else if (events[n].events & EPOLLIN) {
        char buffer[MAX_LEN + 1]; 
        int connfd = events[n].data.fd;
        while (1) {
          memset(buffer, 0, sizeof(buffer));
          int ret = recv(connfd, buffer, MAX_LEN, 0);
          if (ret < 0) {
            if ( (errno == EAGAIN) || (errno == EWOULDBLOCK)) {
              //puts("read later");
              break;
            }
            close(connfd);
            break;
          }
          else if (ret == 0) {
            close(connfd);
          }
          else {
            //printf("%d: %s\n", ret, buffer);
          }
        }
      }
      else if (events[n].events & EPOLLOUT) {
        
      }
    }
  }
}
