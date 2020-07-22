#include <cstring>
#include "Message.h"

Message::Message(int _publisher, int _topic, std::string _message, int _id)
    : publisher(_publisher), 
      topic(_topic),
      message(_message), 
      id(_id) {} 
