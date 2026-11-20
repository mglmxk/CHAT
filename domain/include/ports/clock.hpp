#pragma once
#include <chrono>

class IClock {
public:
    using time_point = std::chrono::system_clock::time_point;
    virtual ~IClock() = default;
    virtual time_point now() const = 0;
};