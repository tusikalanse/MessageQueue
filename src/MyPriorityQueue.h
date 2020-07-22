#ifndef MY_PRIORITY_QUEUE_H
#define MY_PRIORITY_QUEUE_H

#include <queue>
#include <vector>
#include "MQ.h"
#include "PriorityMessage.h"

struct PriorityMessageComparator {
  bool operator()(const std::shared_ptr<Message>& lhs, const std::shared_ptr<Message>& rhs) const {
    return std::dynamic_pointer_cast<PriorityMessage>(lhs)->priority > std::dynamic_pointer_cast<PriorityMessage>(rhs)->priority;
  }
};

class MyPriorityQueue : public MQ {
 public:
  MyPriorityQueue() {};
  ~MyPriorityQueue() {};
  std::shared_ptr<Message> top() const;
  void pop();
  void push(const std::shared_ptr<Message>& message);
  bool empty() const;
  std::size_t size() const;
 private:
  std::priority_queue<std::shared_ptr<Message>, std::vector<std::shared_ptr<Message>>, PriorityMessageComparator> messageQueue;
};

#endif
