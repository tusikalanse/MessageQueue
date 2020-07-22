#ifndef PRIORITY_MESSAGE_H
#define PRIORITY_MESSAGE_H

#include "Message.h"

struct PriorityMessage : public Message {
  int priority; 
  PriorityMessage(int _publisher, int _topic, std::string _message, int _id, int _priority);
  ~PriorityMessage(); 
};

#endif
