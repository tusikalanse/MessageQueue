#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

struct Message {
  int publisher;
  int topic;
  std::string message;
  int id;
  Message(int _publisher, int _topic, std::string _message, int _id);
  virtual ~Message(); 
};

#endif /* MESSAGE_H */
