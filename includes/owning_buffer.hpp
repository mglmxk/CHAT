#pragma once

#include <cstddef>
#include <algorithm>
#include <utility>

class OwningBuffer {
public: 
    OwningBuffer(size_t size) : data_(new char[size]), size_(size){
        std::fill(data_, data_ + size_, '\0');
    }

    ~OwningBuffer() {
        delete[] data_;
    }

    OwningBuffer(const OwningBuffer& other) : data_(new char[other.size_]), size_(other.size_){
        std::copy(other.data_, other.data_ + size_, data_);
    }

    OwningBuffer& operator=(OwningBuffer other) {
        swap(*this, other); 
        return *this;
    }

    OwningBuffer(OwningBuffer&& other) noexcept : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr; 
        other.size_ = 0;
    }

    OwningBuffer& operator=(OwningBuffer&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    friend void swap(OwningBuffer& first, OwningBuffer& second) noexcept { 
        using std::swap;
        swap(first.data_, second.data_);
        swap(first.size_, second.size_); 
    }

    char* data() {
        return data_;
    }

    const char* data() const {
        return data_;
    }

    size_t size() const {
        return size_; 
    }

private: 
    char* data_ = nullptr;
    size_t size_ = 0;
};