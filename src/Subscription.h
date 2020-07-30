#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/hash_policy.hpp>

class Subscription {
 public:
  Subscription() {};
  void addSubscription(const int topic, const int UserID);
  bool removeSubscription(const int topic, const int UserID);
  __gnu_pbds::cc_hash_table<int, bool> getUsers(const int topic);
 private:
  __gnu_pbds::gp_hash_table<int, __gnu_pbds::cc_hash_table<int, bool>> subscription;
};

#endif /* SUBSCRIPTION_H */
