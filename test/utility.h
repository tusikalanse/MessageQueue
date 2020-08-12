#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>
using namespace std;

#define SERVER_IP "127.0.0.1"

#define SERVER_PORT 8000

int getLen(int x) {
  int ans = 1;
  while (x >= 10) ans++, x /= 10;
  return ans;
}

void newMessage(char* dest, const char* message, int topic, int priority = -1) {
  int topicLen = getLen(topic);
  if (priority == -1) {
    sprintf(dest, "POST /message HTTP/1.1\r\nHost: %s:%d\r\nContent-Length: %d\r\n\r\nmessage=%s&topic=%d\r\n", SERVER_IP, SERVER_PORT, strlen(message) + 15 + topicLen, message, topic);
  }
  else {
    int priorityLen = getLen(priority);
    sprintf(dest, "POST /message HTTP/1.1\r\nHost: %s:%d\r\nContent-Length: %d\r\n\r\nmessage=%s&topic=%d&priority=%d\r\n", SERVER_IP, SERVER_PORT, strlen(message) + 25 + topicLen + priorityLen, message, topic, priority);
  }
}

void newSubscription(char* dest, int topic) {
  int topicLen = getLen(topic);
  sprintf(dest, "POST /subscription HTTP/1.1\r\nHost: %s:%d\r\nContent-Length: %d\r\n\r\ntopic=%d\r\n", SERVER_IP, SERVER_PORT, 6 + topicLen, topic);
}

void deleteSubscription(char* dest, int topic) {
  int topicLen = getLen(topic);
  sprintf(dest, "DELETE /subscription HTTP/1.1\r\nHost: %s:%d\r\nContent-Length: %d\r\n\r\ntopic=%d\r\n", SERVER_IP, SERVER_PORT, 6 + topicLen, topic);
}

void ACK(char* dest, int messageID) {
  int IDLen = getLen(messageID);
  sprintf(dest, "PUT /ACK HTTP/1.1\r\nHost: %s:%d\r\nContent-Length: %d\r\n\r\nmessageID=%d\r\n", SERVER_IP, SERVER_PORT, 10 + IDLen, messageID);
}

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

ssize_t socket_send(int sockfd, const char* buffer, size_t buflen) {
	ssize_t tmp;
	size_t total = buflen;
	const char *p = buffer;	
  int cnt;
	while (1) {
		tmp = send(sockfd, p, total, 0);
		if(tmp < 0) {
			if (errno == EINTR)
				return -1;
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
        usleep(1000);
        continue;
      }
			return -1;
    }
		if ((size_t)tmp == total)
			return buflen;
		cnt = 0;
    total -= tmp;
    p += tmp;
	}
	return tmp;
}
