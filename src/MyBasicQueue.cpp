#include "MyBasicQueue.h"

MyBasicQueue::~MyBasicQueue() {}

void MyBasicQueue::pop() {
  MyBasicQueue::messageQueue.pop();
}

void MyBasicQueue::push(const std::shared_ptr<Message>& message) {
  MyBasicQueue::messageQueue.push(message);
}

bool MyBasicQueue::empty() const {
  return MyBasicQueue::messageQueue.empty();
}

int MyBasicQueue::size() const {
  return MyBasicQueue::messageQueue.size();
}