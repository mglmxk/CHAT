#pragma once
#include "ports/event_publisher.hpp"
#include <vector>

class VectorEventPublisher final : public IEventPublisher {
public:
    void publish(const EventEnvelope& e) noexcept override { events_.push_back(e); }
    const std::vector<EventEnvelope>& events() const { return events_; }
    void clear() { events_.clear(); }
private:
    std::vector<EventEnvelope> events_;
};