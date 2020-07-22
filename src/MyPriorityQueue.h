#ifndef MY_PRIORITY_QUEUE_H
#define MY_PRIORITY_QUEUE_H

#include <queue>
#include <vector>
#include "MQ.h"
#include "PriorityMessage.h"

class MyPriorotyQueue : public MQ {
 public:
  MyPriorotyQueue(int priority);
  ~MyPriorotyQueue();
  void pop();
  void push(const std::shared_ptr<Message>& message);
  bool empty();
  int size();
 private:
  constexpr static auto cmp = [] (const std::shared_ptr<Message>& lhs, const std::shared_ptr<Message>& rhs) {
    return std::dynamic_pointer_cast<PriorityMessage>(lhs)->priority > std::dynamic_pointer_cast<PriorityMessage>(rhs)->priority;
  };
  std::priority_queue<std::shared_ptr<Message>, std::vector<std::shared_ptr<Message>>, decltype(cmp)> messageQueue{cmp};
};

#endif
