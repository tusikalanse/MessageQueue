#ifndef CLIENT_H
#define CLIENT_H

#include <cstdio>
#include <queue>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>

struct Client {
  int UserID;
  int socketID;
  char buf[BUFSIZ];
  int readIDX = 0;
  // int checkIDX = 0;
  // int writeIDX = 0;
  std::queue<int> que;
  __gnu_pbds::cc_hash_table<int, bool> subscription;
  Client() = default;
  Client(int _userID, int _socketID) : UserID(_userID), socketID(_socketID) {}
};

#endif /* CLIENT_H */
