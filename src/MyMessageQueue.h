#ifndef MY_MESSAGE_QUEUE_H
#define MY_MESSAGE_QUEUE_H

#include "MQ.h"

class MyMessageQueue {
 public:
  MyMessageQueue();
  MyMessageQueue(int priority);
  ~MyMessageQueue() {};
  std::shared_ptr<Message> top() const;
  void pop();
  void push(const std::shared_ptr<Message>& message);
  bool empty() const;
  std::size_t size() const;
 private:
  std::unique_ptr<MQ> messageQueue;
};

#endif /* MY_MESSAGE_QUEUE_H */
