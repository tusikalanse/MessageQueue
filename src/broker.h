#ifndef BROKER_H
#define BROKER_H

#include "MessageTable.h"
#include "MyMessageQueue.h"
#include "Subscription.h"

class Broker {
 public:
  Broker();
  Broker(int priority);
  Broker(int priority, int _refreshTimeout);
  void ACK(const int messageID, const int UserID);
  void receiveMessage(const std::shared_ptr<Message>& message);
  
 private:
  const int refreshTimeout;
  MyMessageQueue messageQueue;
  MessageTable table; 
  Subscription subscription;
};

#endif
