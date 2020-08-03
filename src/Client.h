#ifndef CLIENT_H
#define CLIENT_H

#include <cstdio>
#include <queue>

struct Client {
  int UserID;
  int socketID;
  char buf[BUFSIZ];
  int readIDX = 0;
  int checkIDX = 0;
  int writeIDX = 0;
  std::queue<int> que;
  Client() = delete;
  Client(int _userID, int _socketID) : UserID(_userID), socketID(_socketID) {}
};

#endif /* CLIENT_H */
