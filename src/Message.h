#ifndef MESSAGE_H
#define MESSAGE_H

struct Message {
  int publisher;
  int topic;
  char* message;
  int id;
  Message(int _publisher, int _topic, char* _message, int _id);
  virtual ~Message(); 
};

#endif
