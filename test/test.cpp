#include "../src/MyPriorityQueue.h"
using namespace std;

int main() {
  MyPriorityQueue pq;
  pq.push(make_shared<PriorityMessage>(PriorityMessage(1, 1, "1", 1, 2)));
  pq.push(make_shared<PriorityMessage>(PriorityMessage(1, 1, "1", 3, 444)));


  return 0;
}