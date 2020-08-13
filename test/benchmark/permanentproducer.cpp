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

int messageNumberPerProducer, messageLenMin, messageLenMax, topicNumber;

int main(int argc, char** argv) {
    assert(argc == 5);
    messageNumberPerProducer = atoi(argv[1]); 
    messageLenMin = atoi(argv[2]);
    messageLenMax = atoi(argv[3]);
    topicNumber = atoi(argv[4]);
    int client_sockfd;
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
      for (int i = 1; i < messageNumberPerProducer; ++i) {
        int len = rand() % messageRandLen + messageLenMin;
        buf[0] = 'a';
        for (int j = 1; j < len; ++j)
        buf[j] = 'a' + rand() % 26;
        buf[len] = '\0';
        int topic = i / messageNumberPerTopic;
        newMessage(message, buf, topic);
        socket_send(client_sockfd, message, strlen(message));
      }
      len = 3;
      buf[0] = 'e', buf[1] = 'n', buf[2] = 'd';
      buf[3] = '\0';
      int topic = topicNumber;
      newMessage(message, buf, topic);
      socket_send(client_sockfd, message, strlen(message));
      sleep(1);
    }    
    close(client_sockfd);
    return 0;
}

