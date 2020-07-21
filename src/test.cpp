#include <bits/extc++.h>
#include <bits/stdc++.h>
using namespace std;
using namespace __gnu_pbds;
#pragma GCC optimize("O0")

const int N = 1e8;

int a[N];
int main() {
    
    clock_t st = clock();
    for (int i = 0; i < N; ++i)
        a[i] = 1;
    cout << (double)(clock() - st) / CLOCKS_PER_SEC << endl;
    st = clock();
    for (int i = 0; i < N; ++i)
        a[i] = 0;
    cout << (double)(clock() - st) / CLOCKS_PER_SEC << endl;
    st = clock();
    for (int i = 0; i < N; i += 8) {
        a[i] = 0;
        a[i + 1] = 0;
        a[i + 2] = 0;
        a[i + 3] = 0;
        a[i + 4] = 0;
        a[i + 5] = 0;
        a[i + 6] = 0;
        a[i + 7] = 0;
    }
    cout << (double)(clock() - st) / CLOCKS_PER_SEC << endl;
    st = clock();
    for (int i = 0; i < N; ++i) {
        a[i] = 1;
        if (a[i] > a[i / 2])
            a[i / 2] = a[i];
    }
    cout << (double)(clock() - st) / CLOCKS_PER_SEC << endl;
// pairing_heap_tag
// thin_heap_tag
// binomial_heap_tag
// rc_binomial_heap_tag 
// binary_heap_tag
    return 0;
}