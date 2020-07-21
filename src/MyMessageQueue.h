#ifndef MY_MESSAGE_QUEUE_H
#define MY_MESSAGE_QUEUE_H
#include "Message.h"

class MyMessageQueue {
 public:
  void pop();
  void push(Message message);
  bool empty();
  int size();
};
        

#endif