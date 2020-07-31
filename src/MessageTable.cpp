#include "MessageTable.h"

void MessageTable::addMessage(const int id, const std::pair<std::shared_ptr<Message>, int>& message) {
  table[id] = message;
}

void MessageTable::ACK(const int messageID) {
  if (table.find(messageID) == table.end())
    return;
  if (--table[messageID].second == 0) {
    table.erase(messageID);
  }
}

std::shared_ptr<Message> MessageTable::getMessage(const int messageID) {
  return table[messageID].first;
}
