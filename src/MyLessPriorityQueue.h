#ifndef MY_LESS_PRIORITY_QUEUE_H
#define MY_LESS_PRIORITY_QUEUE_H

#include <queue>
#include <vector>
#include "MQ.h"
#include "PriorityMessage.h"

class MyLessPriorityQueue : public MQ {
 public:
  MyLessPriorityQueue(int priority);
  ~MyLessPriorityQueue() {};
  std::shared_ptr<Message> top() const;
  void pop();
  void push(const std::shared_ptr<Message>& message);
  bool empty() const;
  std::size_t size() const;
 private:
  std::size_t totalSize;
  std::vector<std::queue<std::shared_ptr<Message>>> messageQueue;
};

#endif /* MY_LESS_PRIORITY_QUEUE_H */