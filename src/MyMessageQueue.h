#ifndef MY_MESSAGE_QUEUE_H
#define MY_MESSAGE_QUEUE_H

#include "Message.h"
#include "MQ.h"

class MyMessageQueue {
 public:
  MyMessageQueue();
  MyMessageQueue(int priority);
  ~MyMessageQueue();
  void pop();
  void push(const std::shared_ptr<Message>& message);
  bool empty();
  int size();
 private:
  std::shared_ptr<MQ> messageQueue;
};

#endif
