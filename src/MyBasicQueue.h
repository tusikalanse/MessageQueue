#ifndef MY_BASIC_QUEUE_H
#define MY_BASIC_QUEUE_H

#include <queue>
#include "MQ.h"

class MyBasicQueue : public MQ {
 public:
  MyBasicQueue() {};
  ~MyBasicQueue() {};
  std::shared_ptr<Message> top() const;
  void pop();
  void push(const std::shared_ptr<Message>& message);
  bool empty() const;
  std::size_t size() const;
 private:
  std::queue<std::shared_ptr<Message>> messageQueue;
};

#endif
