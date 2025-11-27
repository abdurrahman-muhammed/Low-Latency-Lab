\## Single-Producer Single-Consumer (SPSC) Benchmarks



This project implements a custom single-producer single-consumer ring-buffer queue using C++20 atomics, and compares it against `std::queue<int>`.



All benchmarks below are \*\*Release x64\*\*, run with \*\*10,000,000 operations\*\* on my local machine.



\### Single-threaded (synthetic push/pop loops)



| Structure                | Operation | Ops        | Total Time (ms) | Ops/sec        | ns/op   |

|--------------------------|-----------|------------|-----------------|----------------|--------:|

| `std::queue<int>`        | push      | 10,000,000 | 36.74           | 2.72e+08       | 3.67    |

| `std::queue<int>`        | pop       | 10,000,000 | 8.53            | 1.17e+09       | 0.85    |

| `SpscRingBuffer<int>`    | push      | 10,000,000 | 11.56           | 8.65e+08       | 1.16    |

| `SpscRingBuffer<int>`    | pop       | 10,000,000 | 4.92            | 2.03e+09       | 0.49    |



The custom SPSC ring buffer achieves:



\- ~\*\*3× faster push\*\* than `std::queue<int>`  

\- ~\*\*1.7× faster pop\*\* than `std::queue<int>`



This is expected: the ring buffer uses a fixed-size power-of-two capacity, contiguous storage, and simple index arithmetic (`index \& (capacity - 1)`) instead of dynamic allocation and node indirection.



\### Multi-threaded producer–consumer



To simulate a more realistic usage pattern, the SPSC ring buffer is also exercised with:



\- 1 producer thread: pushes exactly \*\*N\*\* items, busy-waiting if the queue is full

\- 1 consumer thread: pops until it has consumed all items, busy-waiting when the queue is empty



```text

=== SPSC ringbuffer MT ===

Ops        : 10000000

Total time : 36.56 ms

Ops/sec    : 2.73e+08

ns/op      : 3.66

Consumed   : 10000000



