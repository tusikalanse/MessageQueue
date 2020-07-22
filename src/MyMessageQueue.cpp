#include "MyBasicQueue.h"
#include "MyLessPriorityQueue.h"
#include "MyMessageQueue.h"
#include "MyPriorityQueue.h"

MyMessageQueue::MyMessageQueue() : messageQueue(std::unique_ptr<MQ>(new MyBasicQueue())) {}

MyMessageQueue::MyMessageQueue(int priority) {
  if (priority <= 1) 
    messageQueue = std::unique_ptr<MQ>(new MyBasicQueue());
  else if (priority <= 10)
    messageQueue = std::unique_ptr<MQ>(new MyLessPriorityQueue(priority));
  else
    messageQueue = std::unique_ptr<MQ>(new MyPriorityQueue());
}

std::shared_ptr<Message> MyMessageQueue::top() const {
  return MyMessageQueue::messageQueue->top();
}

void MyMessageQueue::pop() {
  MyMessageQueue::messageQueue->pop();
}

void MyMessageQueue::push(const std::shared_ptr<Message>& message) {
  MyMessageQueue::messageQueue->push(message);
}

bool MyMessageQueue::empty() const {
  return MyMessageQueue::messageQueue->empty();
}

std::size_t MyMessageQueue::size() const {
  return MyMessageQueue::messageQueue->size();
}