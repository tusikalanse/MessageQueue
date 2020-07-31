// #include <iostream>
// #include "MyDB.h"
// #include "PriorityMessage.h"
// #include <cstdio>

// MyDB::MyDB() {
//   mysql = mysql_init(NULL);
//   if (!mysql) {
//     std::cerr << "Error:" << mysql_error(mysql);
//   }
// }

// MyDB::~MyDB() {
//   if (mysql) {
//     mysql_close(mysql);
//   }
// }

// bool MyDB::connectDB(const std::string& host, const std::string& user, const std::string& pwd, const std::string& dbname, int port) {
//   mysql = mysql_real_connect(mysql, host.c_str(), user.c_str(), pwd.c_str(), dbname.c_str(), port, NULL, 0);
//   if (!mysql) {
//     std::cerr << "Error:" << mysql_error(mysql);
//     return false;
//   }
//   return true;
// }

// bool MyDB::execSQL(const std::string& sql) {
//   if (mysql_query(mysql,sql.c_str())) {
//     std::cerr << "Query Error: " << mysql_error(mysql);
//     return false;
//   }
//   res = mysql_store_result(mysql);

//   if (res) {
//     int               num_fields = mysql_num_fields(res);
//     unsigned long long  num_rows = mysql_num_rows(res);

//     for (unsigned long long i = 0; i < num_rows; ++i) {
//       row = mysql_fetch_row(res);
//       if (!row) break;
//       for (int j = 0; j < num_fields; ++j) {
//         printf("%s\n", row[j]);
//         std::cout<<row[j]<<"\t\t";
//       }
//       std::cout<<std::endl;
//     }
//   } 
//   else {
//     if (mysql_field_count(mysql) == 0) {
//       unsigned long long num_rows = mysql_affected_rows(mysql);
//       return num_rows;
//     } 
//     else {
//       std::cout << "Get res error: " << mysql_error(mysql);
//       return false;
//     }
//   }
//   return true;
// }

// void MyDB::insertMessage(std::shared_ptr<Message>& message) {
//   std::string command = "INSERT INTO `message` VALUES(" + 
//                         std::to_string(message->id) + ", " +
//                         std::to_string(message->publisher) + ", " +
//                         std::to_string(message->topic) + ", " +
//                         message->message;
//   if (typeid(*message) == typeid(PriorityMessage)) {
//     command += ", " + std::to_string(std::dynamic_pointer_cast<PriorityMessage>(message)->priority) + ")";
//   }
//   else {
//     command += ")";
//   }
  
// }