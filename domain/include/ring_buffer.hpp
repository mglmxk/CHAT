#pragma once
#include <vector>
#include <optional>
#include <cstddef>
#include <stdexcept>
#include <utility>

template<typename T>
class RingBuffer {
public:
    explicit RingBuffer(size_t capacity) : capacity_(capacity), slots_(capacity) {
        if (capacity == 0) throw std::invalid_argument("RingBuffer capacity must be > 0");
    }

    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;
    RingBuffer(RingBuffer&&) noexcept = default;
    RingBuffer& operator=(RingBuffer&&) noexcept = default;

    void push_back(T value) {
        if (size_ == capacity_) {
            slots_[head_] = std::move(value);
            head_ = (head_ + 1) % capacity_;
            tail_ = (tail_ + 1) % capacity_;
        } else {
            slots_[tail_] = std::move(value);
            tail_ = (tail_ + 1) % capacity_;
            ++size_;
        }
    }

    void rollback() {
        if (size_ == 0) throw std::runtime_error("Cannot rollback empty RingBuffer");
        tail_ = (tail_ + capacity_ - 1) % capacity_;
        slots_[tail_].reset();
        --size_;
    }

    const T& operator[](size_t i) const {
        if (i >= size_) throw std::out_of_range("RingBuffer index out of range");
        return *slots_[(head_ + i) % capacity_];
    }
    T& operator[](size_t i) {
        if (i >= size_) throw std::out_of_range("RingBuffer index out of range");
        return *slots_[(head_ + i) % capacity_];
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }
    bool empty() const { return size_ == 0; }

private:
    size_t capacity_;
    std::vector<std::optional<T>> slots_;
    size_t head_ = 0, tail_ = 0, size_ = 0;
};