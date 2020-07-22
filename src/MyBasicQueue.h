#ifndef MY_BASIC_QUEUE_H
#define MY_BASIC_QUEUE_H

#include <queue>
#include "Message.h"
#include "MQ.h"

class MyBasicQueue : public MQ {
 public:
  MyBasicQueue();
  ~MyBasicQueue();
  void pop();
  void push(const std::shared_ptr<Message>& message);
  bool empty() const;
  int size() const;
 private:
  std::queue<std::shared_ptr<Message>> messageQueue;
};

#endif
