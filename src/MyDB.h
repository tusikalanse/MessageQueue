#ifndef MYDB_H
#define MYDB_H

#include <mysql/mysql.h>
#include <string>
class MyDB {
 public:
  MyDB();
  ~MyDB();
  bool connectDB(const std::string& host, const std::string& user, const std::string& pwd, const std::string& dbname, int port);
  bool execSQL(const std::string& sql);
 private:
  MYSQL* mysql;
  MYSQL_RES* res;
  MYSQL_ROW row;
};

#endif /* MYDB_H */
