#include <cstring>
#include "Message.h"

Message::Message(int _publisher, int _topic, char* _message, int _id)
    : publisher(_publisher), 
      topic(_topic), 
      id(_id) {
  int len = strlen(_message);
  message = new char[len + 1];
  strncpy(message, _message, len + 1);
} 

Message::~Message() {
  delete[] message;
}
