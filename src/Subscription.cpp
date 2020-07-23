#include "Subscription.h"

void Subscription::addSubscription(const int topic, const int UserID) {
  subscription[topic][UserID] = true;
}

bool Subscription::removeSubscription(const int topic, const int UserID) {
  if (subscription.find(topic) == subscription.end())
    return false;
  if (subscription[topic].find(UserID) == subscription[topic].end())
    return false;
  subscription[topic].erase(UserID);
  return true;
}

__gnu_pbds::cc_hash_table<int, bool> Subscription::getUsers(const int topic) {
  if (subscription.find(topic) == subscription.end())
    return __gnu_pbds::cc_hash_table<int, bool>();
  return subscription[topic];
}
