#pragma once
#include "ports/event_publisher.hpp"

class NullEventPublisher final : public IEventPublisher {
public:
    void publish(const EventEnvelope&) noexcept override {}
};