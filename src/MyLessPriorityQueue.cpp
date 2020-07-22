#include "MyLessPriorityQueue.h"

MyLessPriorityQueue::MyLessPriorityQueue(int priority) : totalSize(0) {
  for (int i = 0; i < priority; ++i) {
    MyLessPriorityQueue::messageQueue.push_back(std::queue<std::shared_ptr<Message>>());
  }
}

std::shared_ptr<Message> MyLessPriorityQueue::top() const {
  for (int i = 0; i < MyLessPriorityQueue::messageQueue.size(); ++i) {
    if (!MyLessPriorityQueue::messageQueue[i].empty())
      return MyLessPriorityQueue::messageQueue[i].front();
  }
  return MyLessPriorityQueue::messageQueue[0].front();
}

void MyLessPriorityQueue::pop() {
  for (int i = 0; i < MyLessPriorityQueue::messageQueue.size(); ++i) {
    if (!MyLessPriorityQueue::messageQueue[i].empty()) {
      totalSize--;
      return MyLessPriorityQueue::messageQueue[i].pop();
    }
  }
  return MyLessPriorityQueue::messageQueue[0].pop();
}

void MyLessPriorityQueue::push(const std::shared_ptr<Message>& message) {
  std::shared_ptr<PriorityMessage> ptr = std::dynamic_pointer_cast<PriorityMessage>(message);
  totalSize++;
  MyLessPriorityQueue::messageQueue[ptr->priority].push(message);
}

bool MyLessPriorityQueue::empty() const {
  return totalSize == 0;
}

int MyLessPriorityQueue::size() const {
  return totalSize;
}