#include <bits/extc++.h>
#include <bits/stdc++.h>
using namespace std;
using namespace __gnu_pbds;
#pragma GCC optimize("O0")

const int N = 1e8;

int a[N];
int main() {
    
    gp_hash_table<int, int> h;
    h[1] = 2;
    h[2] = 1;
    h[5];
    cout << (h.find(9) == h.end()) << endl;
    for (gp_hash_table<int, int>::iterator it = h.begin(); it != h.end(); ++it) {
        cout << it->first << " " << it->second << endl;
    }
// pairing_heap_tag
// thin_heap_tag
// binomial_heap_tag
// rc_binomial_heap_tag 
// binary_heap_tag
    return 0;
}