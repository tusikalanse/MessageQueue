#ifndef MQ_H
#define MQ_H

#include <memory>
#include "Message.h"

class MQ {
 public:
  MQ() {};
  virtual ~MQ() {};
  virtual std::shared_ptr<Message> top() const = 0;
  virtual void pop() = 0;
  virtual void push(const std::shared_ptr<Message>& message) = 0;
  virtual bool empty() const = 0;
  virtual int size() const = 0;
};

#endif
