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
  bool empty() const;
  int size() const;
 private:
  std::unique_ptr<MQ> messageQueue;
};

#endif
