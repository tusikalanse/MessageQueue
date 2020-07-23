#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>

struct Client {
  int id;
  unsigned int ip;
  uint16_t port;
};

#endif
