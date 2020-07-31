#ifndef MESSAGE_TABLE_H
#define MESSAGE_TABLE_H

#include <memory>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include "Message.h"

class MessageTable {
 public:
  void addMessage(const int id, const std::pair<std::shared_ptr<Message>, int>& message);
  void ACK(const int messageID);
  std::shared_ptr<Message> getMessage(const int messageID);
 private:
  __gnu_pbds::gp_hash_table<int, std::pair<std::shared_ptr<Message>, int>> table;
};

#endif /* MESSAGE_TABLE_H */
