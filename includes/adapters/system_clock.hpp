#pragma once
#include "ports/clock.hpp"

class SystemClock final : public IClock {
public:
    time_point now() const override { return std::chrono::system_clock::now(); }
};