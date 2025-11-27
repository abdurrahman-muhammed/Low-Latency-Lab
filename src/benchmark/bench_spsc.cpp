#include "../bench_driver.hpp"
#include "../spsc_queue.hpp"
#include <queue>
#include <thread>
#include <atomic>


BenchResult benchmark_spsc_baseline(std::size_t ops) 
{
    std::queue<int> q;

    auto push_result = run_benchmark("SPSC std::queue push", ops, [&]() 
        {
            q.push(42);
            if (q.size() > 1024) 
            {
                q.pop();
            }
        });


    for (std::size_t i = 0; i < ops; ++i) 
    {
        q.push(42);
    }

    auto pop_result = run_benchmark("SPSC std::queue pop", ops, [&]() 
        {
            if (!q.empty()) q.pop();
        });

    (void)pop_result;
    return push_result;
}

BenchResult benchmark_spsc_ringbuffer(std::size_t ops) {

    SpscRingBuffer<int, 1 << 16> q;

    auto push_result = run_benchmark("SPSC ringbuffer push", ops, [&]() 
        {
            int tmp;
            if (!q.push(42)) 
            {
                q.pop(tmp);
                q.push(42);
            }
        });


    int tmp = 0;
    for (std::size_t i = 0; i < ops; ++i) 
    {
        if (!q.push(42)) 
        {
            q.pop(tmp);
            q.push(42);
        }
    }

    auto pop_result = run_benchmark("SPSC ringbuffer pop", ops, [&]() 
        {
            if (!q.pop(tmp)) 
            {
                // if empty, ignore
            }
        });

    (void)pop_result;
    return push_result;
}

BenchResult benchmark_spsc_ringbuffer_mt(std::size_t ops) 
{
    SpscRingBuffer<int, 1 << 16> q;

    std::atomic<bool> start{ false };
    std::atomic<bool> done{ false };
    std::atomic<std::size_t> consumed{ 0 };

    std::thread producer([&] 
        {
            while (!start.load(std::memory_order_acquire)) 
            {
                // spin until start signal
            }

            for (std::size_t i = 0; i < ops; ++i) 
            {
                while (!q.push(static_cast<int>(i))) 
                {
                    // queue full, spin
                }
            }

            done.store(true, std::memory_order_release);
        });


    std::thread consumer([&]
        {
            while (!start.load(std::memory_order_acquire)) 
            {
                // spin until start signal
            }

            std::size_t local_count = 0;
            int value = 0;

            while (!done.load(std::memory_order_acquire) || !q.empty()) 
            {
                if (q.pop(value)) 
                {
                    ++local_count;
                }
            }

            consumed.store(local_count, std::memory_order_release);
        });

    using clock = std::chrono::steady_clock;
    auto t0 = clock::now();
    start.store(true, std::memory_order_release);

    producer.join();
    consumer.join();
    auto t1 = clock::now();

    std::chrono::duration<double, std::milli> diff = t1 - t0;

    BenchResult r
    {
        "SPSC ringbuffer MT",
        ops,
        diff.count()
    };

    double ops_per_sec = (ops / r.total_ms) * 1000.0;
    double ns_per_op = (r.total_ms * 1'000'000.0) / static_cast<double>(ops);

    std::cout << "=== " << r.name << " ===\n"
        << "Ops        : " << r.ops << "\n"
        << "Total time : " << r.total_ms << " ms\n"
        << "Ops/sec    : " << ops_per_sec << "\n"
        << "ns/op      : " << ns_per_op << "\n"
        << "Consumed   : " << consumed.load() << "\n\n";

    return r;
}

