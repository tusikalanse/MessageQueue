#ifndef MQ_H
#define MQ_H

#include <memory>
#include "Message.h"

class MQ {
 public:
  MQ();
  virtual ~MQ();
  virtual void pop() = 0;
  virtual void push(const std::shared_ptr<Message>& message);
  virtual bool empty();
  virtual int size();
};

#endif
