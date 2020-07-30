#ifndef NOT_ACK_MESSAGE_H
#define NOT_ACK_MESSAGE_H

#include <memory>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include "Message.h"

class NotACKMessage {
 public:
  NotACKMessage();
  NotACKMessage(const std::shared_ptr<Message>& _message, const __gnu_pbds::cc_hash_table<int, bool> _Users);
  bool ACK(const int UserID);
  const __gnu_pbds::cc_hash_table<int, bool>& getUsers() const;
  bool empty() const;
 private:
  std::shared_ptr<Message> message;
  __gnu_pbds::cc_hash_table<int, bool> Users; 
};

#endif /* NOT_ACK_MESSAGE_H */
