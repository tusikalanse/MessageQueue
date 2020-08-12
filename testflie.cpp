#include <bits/stdc++.h>
#include <unistd.h>
using namespace std;

struct vv : vector<int> {
  vv() {}
  vv(const vv& r) {cout << "DD" << endl;}
  vv& operator=(const vv& r) {cout << "DD" << endl;}
  vv(const vv&& r) {cout << "DD" << endl;}
};


char s[] = "122323rr";


int main() { 
  int x = atoi(s + 1);
  cout << x << endl;
  return 0; 
}