#pragma once

#include "ring_buffer.hpp"
#include <iterator>
#include <cstddef>
#include <concepts>

template<typename T>
class HistoryIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    HistoryIterator() = default;

    HistoryIterator(const RingBuffer<T>* buffer, size_t index)
        : buffer_(buffer), index_(index) {}

    reference operator*() const {
        return (*buffer_)[index_];
    }

    pointer operator->() const {
        return &(*buffer_)[index_];
    }

    HistoryIterator& operator++() {
        ++index_;
        return *this;
    }

    HistoryIterator operator++(int) {
        HistoryIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const HistoryIterator& other) const {
        return buffer_ == other.buffer_ && index_ == other.index_;
    }

    bool operator!=(const HistoryIterator& other) const {
        return !(*this == other);
    }

private:
    const RingBuffer<T>* buffer_ = nullptr;
    size_t index_ = 0;
};

template<typename T>
struct HistoryIteratorCheck {
    static_assert(std::forward_iterator<HistoryIterator<T>>, 
        "HistoryIterator must satisfy forward_iterator concept");
};

template<typename T>
class HistoryRange {
public:
    explicit HistoryRange(const RingBuffer<T>& buffer)
        : buffer_(&buffer) {}

    HistoryIterator<T> begin() const {
        return HistoryIterator<T>(buffer_, 0);
    }

    HistoryIterator<T> end() const {
        return HistoryIterator<T>(buffer_, buffer_->size());
    }

private:
    const RingBuffer<T>* buffer_;
};

template<typename T>
HistoryRange<T> history(const RingBuffer<T>& buffer) {
    return HistoryRange<T>(buffer);
}