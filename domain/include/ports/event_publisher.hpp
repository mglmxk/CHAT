#pragma once
#include "event_envelope.hpp"

class IEventPublisher {
public:
    virtual ~IEventPublisher() = default;
    virtual void publish(const EventEnvelope& e) noexcept = 0;
};