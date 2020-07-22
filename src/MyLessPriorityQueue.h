#ifndef MY_LESS_PRIORITY_QUEUE_H
#define MY_LESS_PRIORITY_QUEUE_H

#include <vector>
#include "MQ.h"

class MyLessPriorotyQueue : public MQ {
 public:
  MyLessPriorotyQueue(int priority);
  ~MyLessPriorotyQueue();
  void pop();
  void push(const std::shared_ptr<Message>& message);
  bool empty() const;
  int size() const;
 private:
  std::vector<std::vector<std::shared_ptr<MQ>>> messageQueue;
};

#endif
