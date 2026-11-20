#pragma once

#include "fnv_hash.hpp"
#include <vector>
#include <list>
#include <cstddef>
#include <stdexcept>
#include <utility>

template<typename Key, typename Value, typename Hash = Fnv1aHash>
class HashTable {
public:
    explicit HashTable(size_t initial_capacity = 16)
        : buckets_(initial_capacity), size_(0), hash_fn_() {}
    void insert(const Key& key, Value value) {
        if (static_cast<double>(size_ + 1) / buckets_.size() > 0.75) {
            rehash(buckets_.size() * 2);
        }

        size_t index = hash_fn_(key) % buckets_.size();
        for (auto& [k, v] : buckets_[index]) {
            if (k == key) {
                v = std::move(value);
                return;
            }
        }
        buckets_[index].emplace_back(key, std::move(value));
        ++size_;
    }

    bool contains(const Key& key) const {
        size_t index = hash_fn_(key) % buckets_.size();
        for (const auto& [k, v] : buckets_[index]) {
            if (k == key) return true;
        }
        return false;
    }

    Value& at(const Key& key) {
        size_t index = hash_fn_(key) % buckets_.size();
        for (auto& [k, v] : buckets_[index]) {
            if (k == key) return v;
        }
        throw std::out_of_range("Key not found in HashTable");
    }

    const Value& at(const Key& key) const {
        size_t index = hash_fn_(key) % buckets_.size();
        for (const auto& [k, v] : buckets_[index]) {
            if (k == key) return v;
        }
        throw std::out_of_range("Key not found in HashTable");
    }

    size_t size() const { return size_; }
    size_t capacity() const { return buckets_.size(); }
    double load_factor() const { 
        return static_cast<double>(size_) / buckets_.size(); 
    }

    struct Iterator {
        using BucketIter = typename std::list<std::pair<Key, Value>>::const_iterator;
        
        const std::vector<std::list<std::pair<Key, Value>>>* buckets;
        size_t bucket_index;
        BucketIter bucket_iter;

        Iterator(const std::vector<std::list<std::pair<Key, Value>>>* b, 
                 size_t idx, BucketIter it)
            : buckets(b), bucket_index(idx), bucket_iter(it) {}

        const std::pair<Key, Value>& operator*() const { return *bucket_iter; }
        const std::pair<Key, Value>* operator->() const { return &(*bucket_iter); }

        Iterator& operator++() {
            ++bucket_iter;
            if (bucket_iter == (*buckets)[bucket_index].end()) {
                ++bucket_index;
                while (bucket_index < buckets->size() && 
                       (*buckets)[bucket_index].empty()) {
                    ++bucket_index;
                }
                if (bucket_index < buckets->size()) {
                    bucket_iter = (*buckets)[bucket_index].begin();
                }
            }
            return *this;
        }

        bool operator==(const Iterator& other) const {
            if (bucket_index != other.bucket_index) return false;
            if (bucket_index == buckets->size()) return true;   
            return bucket_iter == other.bucket_iter;
        }
        bool operator!=(const Iterator& other) const { return !(*this == other); }
    };

    Iterator begin() const {
        for (size_t i = 0; i < buckets_.size(); ++i) {
            if (!buckets_[i].empty()) {
                return Iterator(&buckets_, i, buckets_[i].begin());
            }
        }
        return end();
    }

    Iterator end() const {
        using BucketIter = typename std::list<std::pair<Key, Value>>::const_iterator;
        return Iterator(&buckets_, buckets_.size(), BucketIter());
    }

    size_t chain_length(size_t bucket_index) const {
        if (bucket_index >= buckets_.size()) return 0;
        return buckets_[bucket_index].size();
    }

private:
    std::vector<std::list<std::pair<Key, Value>>> buckets_;
    size_t size_;
    Hash hash_fn_;

    void rehash(size_t new_capacity) {
        std::vector<std::list<std::pair<Key, Value>>> new_buckets(new_capacity);
        for (auto& bucket : buckets_) {
            for (auto& pair : bucket) {
                size_t index = hash_fn_(pair.first) % new_capacity;
                new_buckets[index].emplace_back(pair.first, std::move(pair.second));
            }
        }
        buckets_ = std::move(new_buckets);
    }
};