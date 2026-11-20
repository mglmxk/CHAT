#pragma once

#include <cstddef>
#include <string_view>

struct BadHash {
    size_t operator()(std::string_view str) const {
        size_t hash = 0;
        for (char c : str) {
            hash += static_cast<size_t>(c);
        }
        return hash;
    }
};
