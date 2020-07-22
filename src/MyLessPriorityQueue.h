#ifndef MY_LESS_PRIORITY_QUEUE_H
#define MY_LESS_PRIORITY_QUEUE_H

#include "Message.h"
#include "MQ.h"

class MyLessPriorotyQueue : public MQ {
 public:
  MyLessPriorotyQueue(int priority);
  ~MyLessPriorotyQueue();
  void pop();
  void push(const std::shared_ptr<Message>& message);
  bool empty();
  int size();
 private:
  MQ** messageQueue;
};

#endif
