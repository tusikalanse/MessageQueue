// #include <bits/extc++.h>
// #include <bits/stdc++.h>
// using namespace std;
// #pragma GCC optimize("O0")

// const int N = 1e8;

// struct foo {
//   foo() {}
//   foo(int _a) : a(_a) {}
//   int a;
// };

// struct FooComparator {
//   auto operator()(const std::shared_ptr<foo>& lhs, const std::shared_ptr<foo>& rhs) const 
//     -> bool
//   { return lhs->a > rhs->a; }
// };

// int a[N];
// int main() {
//   auto cmp = [] (const std::shared_ptr<foo>& lhs, const std::shared_ptr<foo>& rhs) {return lhs->a > rhs->a;};
//   std::priority_queue<std::shared_ptr<foo>, vector<std::shared_ptr<foo>>, FooComparator> pq;
//   std::shared_ptr<foo> x[100];
//   for (int i = 0; i < 100; ++i) {
//     x[i] = std::make_shared<foo>(rand() % 10);
//     pq.push(x[i]);
//   }
//   for (int i = 0; i < 100; ++i) {
//     std::shared_ptr<foo> x = pq.top();
//     pq.pop();
//     cout << x->a << endl;
//   }
//   return 0;
// }


// #include <bits/stdc++.h>
// using namespace std;

// struct foo {
//   virtual int hi() {cout << "dsds" << endl;}
// };

// struct bar : public foo {
//   int hi() {cout << "ddd" << endl;}
//   int hh() {cout << "ddddddd" << endl;}
// };

// int main() {
//   std::shared_ptr<foo> x = std::make_shared<bar>(bar());
//   x->hi();
//   std::shared_ptr<bar> xx = std::dynamic_pointer_cast<bar>(x);
//   xx->hh();

//   return 0;
// }

#include <bits/stdc++.h>
#include "MyPriorityQueue.h"
using namespace std;

int main() {
  MyPriorityQueue MQ;
  for (int i = 1; i <= 100; ++i) {
    MQ.push(std::shared_ptr<Message>(new PriorityMessage(1, 2, to_string(rand()), 1, rand() % 10)));
  }
  for (int i = 1; i <= 100; ++i) {
    shared_ptr<PriorityMessage> M = std::dynamic_pointer_cast<PriorityMessage>(MQ.top());
    MQ.pop();
    cout << MQ.size() << " " << boolalpha << MQ.empty() << " ";
    cout << M->message << " " << M->priority << endl;
  }
  return 0;
}