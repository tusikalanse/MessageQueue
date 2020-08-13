#include <benchmark/benchmark.h>

static void bench_producer_1_100000_10_20(benchmark::State& state) {
    for (auto _: state) {
        system("bash testproducer.sh 1 100000 10 20 1");
    }
}
BENCHMARK(bench_producer_1_100000_10_20);

static void bench_producer_10_100000_10_20(benchmark::State& state) {
    for (auto _: state) {
        system("bash testproducer.sh 10 100000 10 20 1");
    }
}
BENCHMARK(bench_producer_10_100000_10_20);

static void bench_producer_10_100000_100_200(benchmark::State& state) {
    for (auto _: state) {
        system("bash testproducer.sh 10 100000 100 200 1");
    }
}
BENCHMARK(bench_producer_10_100000_100_200);

static void bench_producer_10_100000_1000_2000(benchmark::State& state) {
    for (auto _: state) {
        system("bash testproducer.sh 10 100000 1000 2000 1");
    }
}
BENCHMARK(bench_producer_10_100000_1000_2000);

static void bench_producer_100_10000_1000_2000(benchmark::State& state) {
    for (auto _: state) {
        system("bash testproducer.sh 100 10000 1000 2000 1");
    }
}
BENCHMARK(bench_producer_100_10000_1000_2000);

BENCHMARK_MAIN();