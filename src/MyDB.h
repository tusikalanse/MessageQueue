// #ifndef MYDB_H
// #define MYDB_H

// #include <mysql/mysql.h>
// #include <string>
// #include <ext/pb_ds/assoc_container.hpp>
// #include <ext/pb_ds/hash_policy.hpp>
// #include "Message.h"

// class MyDB {
//  public:
//   MyDB();
//   ~MyDB();
//   bool connectDB(const std::string& host, const std::string& user, const std::string& pwd, const std::string& dbname, int port);
//   bool execSQL(const std::string& sql);
//   __gnu_pbds::cc_hash_table<int, bool> getUsers(const int topic);
//   void insertMessage(std::shared_ptr<Message>& message);
//   void deleteMessage(std::shared_ptr<Message>& message);
//  private:
//   MYSQL* mysql;
//   MYSQL_RES* res;
//   MYSQL_ROW row;
// };

// #endif /* MYDB_H */
