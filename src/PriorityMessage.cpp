#include "PriorityMessage.h"

PriorityMessage::PriorityMessage(int _publisher, int _topic, std::string _message, int _id, int _priority)
    : Message(_publisher, _topic, _message, _id),
      priority(_priority) {}
