// #include "network.h"

// #include <arpa/inet.h>
// #include <cstdio>
// #include <cstring>
// #include <errno.h>
// #include <fcntl.h>
// #include <iostream>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <unistd.h>

// void network::setnonblocking(int sockfd) {
//   int flag = fcntl(sockfd, F_GETFL, 0);
//   if (flag < 0) {
//     perror("fcntl F_GETFL fail");
//     return;
//   }
//   if (fcntl(sockfd, F_SETFL, flag | O_NONBLOCK) < 0) {
//     perror("fcntl F_SETFL fail");
//   }
// }

// int network::read(Client& client) {
//   int sockfd = client.socketID;
//   char* buf = client.buf + client.readIDX;
//   int res = 0;
//   while (1) {
//     int ret = recv(sockfd, buf, BUFSIZ - client.readIDX - 10, 0);
//     if (ret < 0) {
//       if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
//         break;
//       }
//       close(sockfd);
//       return 0;
//     }
//     else if (ret == 0) {
//       close(sockfd);
//       return 0;
//     }  
//     else {
//       buf[ret] = '\0';
//       res += ret;
//       //HTTPParser(client);
//       //printf("%d %d: %s\n", ret, strlen(buf), buf);
//       //fflush(stdout);
//     }
//   }
//   return res;
// }

// int network::send(Client& client, const char* buf, int len) {
//   int sockfd = client.socketID;
//   int pos = 0;
//   while (pos < len) {
//     int ret = ::send(sockfd, buf + pos, len - pos, 0);
//     if (-1 == ret) {
//       if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
//         ret = 0;
//         continue;
//       }
//       return 0;
//     }
//     pos += ret;
//   }
//   return pos;
// }

// void network::HTTPParser(Client& client) {
//   int n = strlen(client.buf), IDX = 0;
//   if (findIndex(client.buf) != 0) {
//     std::cerr << "error" << std::endl;
//   }
//   while (IDX < n) {
//     if (client.buf[IDX] == 'G') {
//       const char* temp = strstr(client.buf + IDX, "\r\n\r\n");
//       if (temp == NULL) {
//         strncpy(client.buf, client.buf + IDX, n - IDX);
//         client.readIDX = n - IDX;
//         return;
//       }
//       dealGet(client.buf + IDX, temp - client.buf + 4 - IDX);
//       IDX = temp - client.buf + 4;
//     }
//     else {
//       const char* temp = strstr(client.buf + IDX, "Content-Length:");
//       if (temp == NULL) {
//         strncpy(client.buf, client.buf + IDX, n - IDX);
//         client.readIDX = n - IDX;
//         return;
//       }
//       int length = 0;
//       char ch = *temp;
//       while (ch < '0' || ch > '9') ch = *++temp;
//       while (ch >= '0' && ch <= '9') {
//         length = length * 10 + ch - '0';
//         ch = *++temp;
//       }
//       temp = strstr(temp, "\r\n\r\n");
//       if (temp == NULL || temp + 4 + length > client.buf + n) {
//         strncpy(client.buf, client.buf + IDX, n - IDX);
//         client.readIDX = n - IDX;
//         return;
//       }
//       if (client.buf[IDX] == 'D') 
//         dealDelelte(client.buf + IDX, temp + 4, length);
//       else if (client.buf[IDX + 1] == 'O')
//         dealPost(client.buf + IDX, temp + 4, length);
//       else if (client.buf[IDX + 1] == 'U')
//         dealPut(client.UserID, client.buf + IDX, temp + 4, length);
//       else
//         if (IDX != n) std::cerr << "Bad Request" << std::endl;
//       IDX = temp - client.buf + length + 4;
//     }
//   }
//   if (IDX != n) std::cerr << "error IDX != n" << std::endl;
//   client.readIDX = 0;
// }

// int network::dealGet(const char* buf, int len) {
//   //todo
// }


// int network::dealPost(const char* buf, const char* body, int len) {
//   //todo
// }

// int network::dealPut(int UserID, const char* buf, const char* body, int len) {
//   //url: .../ACK
//   //body: messageID=$ID
//   const char* temp = strchr(buf, ' ');
//   if (strstr(temp, "ACK") != temp + 1) return 400;
//   if (strstr(body, "messageID=") != body) return 400;
//   temp = strchr(body, '=') + 1;
//   int id = atoi(temp);
  


// }

// int network::dealDelelte(const char* buf, const char* body, int len) {
//   //todo
// }

// int network::findIndex(const char* buf) {
//   const char* result;
//   int res = 1e9;
//   result = strstr(buf, "GET");
//   if (result != NULL && result - buf < res) {
//     res = result - buf;
//   }
//   result = strstr(buf, "POST");
//   if (result != NULL && result - buf < res) {
//     res = result - buf;
//   }
//   result = strstr(buf, "PUT");
//   if (result != NULL && result - buf < res) {
//     res = result - buf;
//   }
//   result = strstr(buf, "DELETE");
//   if (result != NULL && result - buf < res) {
//     res = result - buf;
//   }
//   if (res == 1e9) return -1;
//   return res;
// }