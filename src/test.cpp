#include <bits/extc++.h>
#include <bits/stdc++.h>
using namespace std;
using namespace __gnu_pbds;


const int N = 1e8;

int a[N];
int main() {
    srand(time(NULL));
    std::vector<int> v;
    for (int i = 0; i < N; ++i)
        v.push_back(rand());
    queue<int> pq0;
    std::priority_queue<int, vector<int>, greater<int> > pq1;
    __gnu_pbds::priority_queue<int,greater<int>,pairing_heap_tag> pq2;
    __gnu_pbds::priority_queue<int,greater<int>,thin_heap_tag> pq3;
    __gnu_pbds::priority_queue<int,greater<int>,binomial_heap_tag> pq4;
    __gnu_pbds::priority_queue<int,greater<int>,rc_binomial_heap_tag> pq5;
    clock_t st = clock();
    for (int i = 0; i < N; ++i) {
		pq0.push(v[i]);
	}
    for (int i = 0; i < N; ++i) {
		pq0.pop();
	}
	cout << (double)(clock() - st) / CLOCKS_PER_SEC << endl;
    st = clock();
    int f = 0, end = -1;
    for (int i = 0; i < N; ++i) {
        a[++end] = v[i];
    }
    for (int i = 0; i < N; ++i) 
        f++;
    cout << (double)(clock() - st) / CLOCKS_PER_SEC << endl;
// pairing_heap_tag
// thin_heap_tag
// binomial_heap_tag
// rc_binomial_heap_tag 
// binary_heap_tag
    return 0;
}