#include <iostream>
#include <string>
#include <cstddef>

#include "bench_driver.hpp"

BenchResult benchmark_spsc_baseline(std::size_t ops);
BenchResult benchmark_spsc_ringbuffer(std::size_t ops);
BenchResult benchmark_spsc_ringbuffer_mt(std::size_t ops);

int main(int argc, char** argv) 
{
    std::string bench = "spsc";
    std::size_t ops = 10'000'000; 

    if (argc > 1) bench = argv[1];
    if (argc > 2) ops = static_cast<std::size_t>(std::stoull(argv[2]));

    std::cout << "Running benchmark: " << bench << " with ops=" << ops << "\n\n";

    if (bench == "spsc") 
    {
        auto r1 = benchmark_spsc_baseline(ops);
        auto r2 = benchmark_spsc_ringbuffer(ops);
        (void)r1;
        (void)r2;
    }
    else if (bench == "spsc_mt")
    {
        auto r3 = benchmark_spsc_ringbuffer_mt(ops);
        (void)r3;
    }
    else 
    {
        std::cerr << "Unknown benchmark: " << bench << "\n";
        return 1;
    }

    return 0;
}