#ifndef MY_MESSAGE_QUEUE_H
#define MY_MESSAGE_QUEUE_H
#include "Message.h"

class MyMessageQueue {
 public:
  MyMessageQueue();
  MyMessageQueue(int priority);
  ~MyMessageQueue();
  void pop();
  void push(Message* message);
  bool empty();
  int size();
 private:
  MQ* messageQueue;
};

#endif
