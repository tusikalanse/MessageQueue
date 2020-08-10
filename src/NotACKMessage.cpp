// #include "NotACKMessage.h"

// NotACKMessage::NotACKMessage() {};

// NotACKMessage::NotACKMessage(const std::shared_ptr<Message>& _message, const __gnu_pbds::cc_hash_table<int, bool> _Users) :
//     message(_message), 
//     Users(_Users)
//   {}

// bool NotACKMessage::ACK(const int UserID) {
//   if (Users.find(UserID) == Users.end())
//     return false;
//   Users.erase(UserID);
//   return true;
// }

// const __gnu_pbds::cc_hash_table<int, bool>& NotACKMessage::getUsers() const {
//   return Users;
// }

// bool NotACKMessage::empty() const {
//   return Users.empty();
// }
