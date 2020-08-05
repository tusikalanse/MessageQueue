#include <iostream>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;


list<int> clients_list;

/***** macro defintion *****/
//server ip
#define SERVER_IP "9.134.237.210"

//server port
#define SERVER_PORT 8000

//epoll size
#define EPOLL_SIZE 5000

//message buffer size
#define BUF_SIZE 0xFFFF

#define SERVER_WELCOME "Welcome you join to the chat room! Your chat ID is: Client #%d"

#define SERVER_MESSAGE "ClientID %d say >> %s"

//exit
#define EXIT "EXIT"

#define CAUTION "There is only ont int the char root!"

/****** some function *****/
/**
 *设置非阻塞
 */
int setnonblockint(int sockfd) {
    fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
    return 0;
}

/**
 * 将文件描述符 fd 添加到 epollfd 标示的内核事件表中,
 * 并注册 EPOLLIN 和 EPOOLET 事件,
 * EPOLLIN 是数据可读事件；EPOOLET 表明是 ET 工作方式。
 * 最后将文件描述符设置非阻塞方式
 * @param epollfd:epoll句柄
 * @param fd:文件描述符
 * @param enable_et:enable_et = true,
 * 是否采用epoll的ET(边缘触发)工作方式；否则采用LT(水平触发)工作方式
 */
void addfd(int epollfd, int fd, bool enable_et) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if (enable_et) {
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
    setnonblockint(fd);
    printf("fd added to epoll!\n\n");
}

//发送广播
int sendBroadcastmessage(int clientfd) {
    char buf[BUF_SIZE];
    char message[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    bzero(buf, BUF_SIZE);

    printf("read from client(clientID = %d)\n", clientfd);
    int len = recv(clientfd, buf, BUF_SIZE, 0);

    if (0 == len) {
        close(clientfd);
        clients_list.remove(clientfd);
        printf("ClientID = %d closed.\n now there are %d client in the char room\n",
        clientfd, (int)clients_list.size());
    } else {
        if (1 == clients_list.size()) {
            send(clientfd, CAUTION, strlen(CAUTION), 0);
            return 0;
        }
        sprintf(message, SERVER_MESSAGE, clientfd, buf);
        list<int>::iterator it;
        for (it = clients_list.begin(); it != clients_list.end(); ++it) {
            if (*it != clientfd) {
                if (send(*it, message, BUF_SIZE, 0) < 0) {
                    perror("error");
                    exit(-1);
                }
            }
        }
    }
    return len;
}

#define error(msg) \
    do {perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char *argv[]) {
    /**
     * TCP 客户端通信
     * 1.创建套接字（socket）
     * 2.使用 connect() 建立到达服务器的连接（connect)
     * 3.客户端进行通信（使用 write()/send() 或 send()/recv() )
     * 4.使用 close() 关闭客户连接
     */

    /**
     * 1:创建套接字socket
     * param1:指定地址族为IPv4;param2:指定传输协议为流式套接字;param3:指定传输协议为TCP,可设为0,由系统推导
     */
    int clientfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientfd < 0) { error("socket error"); }

    // 填充sockadd结构,指定ip与端口
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 2:连接服务端
    if (connect(clientfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        error("connect error");
    }

    // 创建管道，其中fd[0]用于父进程读，fd[1]用于子进程写
    int pipefd[2];
    if (pipe(pipefd) < 0) { error("pipe error"); }

    /**
     * epoll使用
     * 1:调用 epoll_create 函数在 Linux 内核中创建一个事件表；
     * 2:然后将文件描述符添加到所创建的事件表中 (epoll_ctl)；
     * 3:在主循环中，调用 epoll_wait 等待返回就绪的文件描述符集合；
     * 4:分别处理就绪的事件集合
     */
    // 创建epoll
    int epfd = epoll_create(EPOLL_SIZE);
    if (epfd < 0) { error("epfd error"); }

    static struct epoll_event events[2];
    //将sock和管道读端描述符都添加到内核事件表中
    addfd(epfd, clientfd, true);
    addfd(epfd, pipefd[0], true);

    // 表示客户端是否正常工作
    bool isClientwork = true;

    // 聊天信息缓冲区
    char message[BUF_SIZE];

    // Fork
    int pid = fork();
    if (pid < 0) {
        error("fork error");
    } else if (pid == 0) {      // 子进程
        //子进程负责写入管道，因此先关闭读端
        close(pipefd[0]);
        printf("Please input 'exit' to exit the chat room\n");

        while (isClientwork) {
            bzero(&message, BUF_SIZE);
            fgets(message, BUF_SIZE, stdin);

            // 客户输出exit,退出
            if (strncasecmp(message, EXIT, strlen(EXIT)) == 0) {
                isClientwork = 0;
            } else {    // 子进程将信息写入管道
                message[strlen(message) - 1] = '\0';
                if (write(pipefd[1], message, strlen(message)) < 0) {
                    error("fork error");
                }
                puts("writing");
            }
        }
    } else { //pid > 0 父进程
        //父进程负责读管道数据，因此先关闭写端
        close(pipefd[1]);

        // 主循环(epoll_wait)
        while (isClientwork) {
            // 等待事件的产生，函数返回需要处理的事件数目
            int epoll_events_count = epoll_wait(epfd, events, 2, -1);
            // 处理就绪事件
            for (int i = 0; i < epoll_events_count; ++i) {
                bzero(&message, BUF_SIZE);

                //服务端发来消息
                if (events[i].data.fd == clientfd) {
                    //接受服务端消息
                    int ret = recv(clientfd, message, BUF_SIZE, 0);

                    // ret= 0 服务端关闭
                    if (ret == 0) {
                        printf("Server closed connection: %d\n", clientfd);
                        close(clientfd);
                        isClientwork = 0;
                    } else printf("%s\n", message);
                }
                    //子进程写入事件发生，父进程处理并发送服务端
                else {
                    //父进程从管道中读取数据
                    int ret = read(events[i].data.fd, message, BUF_SIZE);

                    // ret = 0
                    if (ret == 0) {
                        isClientwork = 0;
                    } else {   // 将信息发送给服务端
                        puts("sending data");
                        send(clientfd, message, strlen(message), 0);
                    }
                }
            }//for
        }//while
    }

    if (pid) {
        //关闭父进程和sock
        close(pipefd[0]);
        close(clientfd);
    } else {
        //关闭子进程
        close(pipefd[1]);
    }
    return 0;
}