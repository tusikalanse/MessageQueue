#include "MyPriorityQueue.h"

std::shared_ptr<Message> MyPriorityQueue::top() const {
  return MyPriorityQueue::messageQueue.top();
}

void MyPriorityQueue::pop() {
  MyPriorityQueue::messageQueue.pop();
}

void MyPriorityQueue::push(const std::shared_ptr<Message>& message) {
  MyPriorityQueue::messageQueue.push(message);
}

bool MyPriorityQueue::empty() const {
  return MyPriorityQueue::messageQueue.empty();
}

std::size_t MyPriorityQueue::size() const {
  return MyPriorityQueue::messageQueue.size();
}