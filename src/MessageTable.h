#ifndef MESSAGE_TABLE_H
#define MESSAGE_TABLE_H

#include <memory>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>
#include "NotACKMessage.h"

class MessageTable {
 public:
  void addMessage(const int id, const std::shared_ptr<NotACKMessage>& message);
  void ACK(const int messageID, const int UserID);
  __gnu_pbds::gp_hash_table<int, std::shared_ptr<NotACKMessage>>& getHashTable();
 private:
  __gnu_pbds::gp_hash_table<int, std::shared_ptr<NotACKMessage>> table;
};

#endif
