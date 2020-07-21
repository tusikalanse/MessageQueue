#ifndef PRIORITY_MESSAGE_H
#define PRIORITY_MESSAGE_H
#include "Message.h"

struct PriorityMessage : public Message {
  int priority; 
  Message(int _publisher, int _topic, char* _message, int _id, int _priority);
  ~Message(); 
};

#endif
