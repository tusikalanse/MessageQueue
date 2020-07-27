#include "Broker.h"

Broker::Broker() : 
    refreshTimeout(1), 
    messageQueue(), 
    table(), 
    subscription()
  {}

Broker::Broker(int priority) : 
    refreshTimeout(1), 
    messageQueue(priority), 
    table(), 
    subscription() 
  {}

Broker::Broker(int priority, int _refreshTimeout) : 
    refreshTimeout(_refreshTimeout), 
    messageQueue(priority), 
    table(), 
    subscription() 
  {}

