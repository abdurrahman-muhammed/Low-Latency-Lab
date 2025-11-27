#pragma once

#include <atomic>
#include <cstddef>
#include <array>


template<typename T, std::size_t Capacity>
class SpscRingBuffer 
{
    static_assert((Capacity& (Capacity - 1)) == 0, "Capacity must be a power of two (e.g. 1024, 65536)");

public:
    SpscRingBuffer() : head_(0), tail_(0) {}

    SpscRingBuffer(const SpscRingBuffer&) = delete;
    SpscRingBuffer& operator=(const SpscRingBuffer&) = delete;


    bool push(const T& value) {
        auto head = head_.load(std::memory_order_relaxed);
        auto tail = tail_.load(std::memory_order_acquire);
        std::size_t next = (head + 1) & mask_;

        if (next == tail) 
        {
            return false;
        }

        buffer_[head] = value;
        head_.store(next, std::memory_order_release);
        return true;
    }

    bool pop(T& out) {
        auto tail = tail_.load(std::memory_order_relaxed);
        auto head = head_.load(std::memory_order_acquire);

        if (tail == head) 
        {
            return false;
        }

        out = buffer_[tail];
        std::size_t next = (tail + 1) & mask_;
        tail_.store(next, std::memory_order_release);
        return true;
    }

    bool empty() const {
        auto tail = tail_.load(std::memory_order_relaxed);
        auto head = head_.load(std::memory_order_relaxed);
        return tail == head;
    }

    bool full() const {
        auto head = head_.load(std::memory_order_relaxed);
        auto tail = tail_.load(std::memory_order_relaxed);
        return ((head + 1) & mask_) == tail;
    }

    std::size_t capacity() const 
    {
        return Capacity - 1;
    }

private:
    static constexpr std::size_t mask_ = Capacity - 1;

    std::array<T, Capacity> buffer_;
    std::atomic<std::size_t> head_; 
    std::atomic<std::size_t> tail_; 
};