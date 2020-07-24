#include "MessageTable.h"

void MessageTable::addMessage(const int id, const std::shared_ptr<NotACKMessage>& message) {
  table[id] = message;
}

void MessageTable::ACK(const int messageID, const int UserID) {
  if (table.find(messageID) == table.end())
    return;
  table[messageID]->ACK(UserID);
  
}

__gnu_pbds::gp_hash_table<int, std::shared_ptr<NotACKMessage>>& MessageTable::getHashTable() {
  return table;
}
