#include <benchmark/benchmark.h>
#include <array>
#include <iostream>
using namespace std;

// constexpr int len = 6;

// // constexpr function具有inline属性，你应该把它放在头文件中
// constexpr auto my_pow(const int i)
// {
//     return i * i;
// }

// // 使用operator[]读取元素，依次存入1-6的平方
// static void bench_array_operator(benchmark::State& state)
// {
//     std::array<int, len> arr;
//     constexpr int i = 1;
//     for (auto _: state) {
//         arr[0] = my_pow(i);
//         arr[1] = my_pow(i+1);
//         arr[2] = my_pow(i+2);
//         arr[3] = my_pow(i+3);
//         arr[4] = my_pow(i+4);
//         arr[5] = my_pow(i+5);
//     }
// }
// BENCHMARK(bench_array_operator);

// // 使用at()读取元素，依次存入1-6的平方
// static void bench_array_at(benchmark::State& state)
// {
//     std::array<int, len> arr;
//     constexpr int i = 1;
//     for (auto _: state) {
//         arr.at(0) = my_pow(i);
//         arr.at(1) = my_pow(i+1);
//         arr.at(2) = my_pow(i+2);
//         arr.at(3) = my_pow(i+3);
//         arr.at(4) = my_pow(i+4);
//         arr.at(5) = my_pow(i+5);
//     }
// }
// BENCHMARK(bench_array_at);

// // std::get<>(array)是一个constexpr function，它会返回容器内元素的引用，并在编译期检查数组的索引是否正确
// static void bench_array_get(benchmark::State& state)
// {
//     std::array<int, len> arr;
//     constexpr int i = 1;
//     for (auto _: state) {
//         std::get<0>(arr) = my_pow(i);
//         std::get<1>(arr) = my_pow(i+1);
//         std::get<2>(arr) = my_pow(i+2);
//         std::get<3>(arr) = my_pow(i+3);
//         std::get<4>(arr) = my_pow(i+4);
//         std::get<5>(arr) = my_pow(i+5);
//     }
// }
// BENCHMARK(bench_array_get);

// static void bench_simple_producer1000(benchmark::State& state) {
//     for (auto _: state) {
//         system("./testproducer 1000 10 20 10");
//     }
// }

// BENCHMARK(bench_simple_producer1000)->Iterations(10);

// static void bench_simple_producer100000(benchmark::State& state) {
//     for (auto _: state) {
//         system("./testproducer 100000 10 20 10");
//     }
// }

// BENCHMARK(bench_simple_producer100000)->Iterations(10);

// static void bench_simple_consumer(benchmark::State& state) {
//     for (auto _: state) {
//         system("./testconsumer 2 2 100000 &");
//     }
// }

// BENCHMARK(bench_simple_consumer);

static void bench_test(benchmark::State& state) {
    cout << "dd" << endl;
    for (auto _: state) {
        //cout << "dd" << endl;
    }
}

BENCHMARK(bench_test);

BENCHMARK_MAIN();