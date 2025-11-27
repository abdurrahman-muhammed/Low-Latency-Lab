#pragma once

#include <chrono>
#include <iostream>
#include <string>

struct BenchResult 
{
    std::string name;
    std::size_t ops;
    double total_ms;
};

template<typename F>
BenchResult run_benchmark(const std::string& name, std::size_t ops, F&& func) 
{
    using clock = std::chrono::steady_clock;
    auto start = clock::now();
    for (std::size_t i = 0; i < ops; ++i) 
    {
        func();
    }
    auto end = clock::now();
    std::chrono::duration<double, std::milli> diff = end - start;

    BenchResult r{ name, ops, diff.count() };

    double ops_per_sec = (ops / r.total_ms) * 1000.0;
    double ns_per_op = (r.total_ms * 1'000'000.0) / static_cast<double>(ops);

    std::cout << "=== " << name << " ===\n"
        << "Ops        : " << ops << "\n"
        << "Total time : " << r.total_ms << " ms\n"
        << "Ops/sec    : " << ops_per_sec << "\n"
        << "ns/op      : " << ns_per_op << "\n\n";

    return r;
}