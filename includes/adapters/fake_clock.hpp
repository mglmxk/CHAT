#pragma once
#include "ports/clock.hpp"

class FakeClock final : public IClock {
public:
    explicit FakeClock(time_point start = time_point{}) : now_(start) {}
    time_point now() const override { return now_; }
    void advance(std::chrono::seconds d) { now_ += d; }
private:
    time_point now_;
};