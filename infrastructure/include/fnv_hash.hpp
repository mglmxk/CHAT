#pragma once

#include <cstddef>
#include <string>
#include <string_view>

struct Fnv1aHash {
    static constexpr size_t OFFSET_BASIS = 14695981039346656037ULL;
    static constexpr size_t PRIME = 1099511628211ULL;

    size_t operator()(std::string_view str) const {
        size_t hash = OFFSET_BASIS;
        for (char c : str) {
            hash ^= static_cast<size_t>(c);
            hash *= PRIME;
        }
        return hash;
    }
};