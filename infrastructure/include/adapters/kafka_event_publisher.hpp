#pragma once
#include "ports/event_publisher.hpp"
#include "kafka_producer.hpp"
#include <memory>

class KafkaEventPublisher final : public IEventPublisher {
public:
    KafkaEventPublisher(const std::string& brokers, const std::string& topic)
        : producer_(std::make_unique<KafkaProducer>(brokers, topic)) {}

    void publish(const EventEnvelope& e) noexcept override {
        try { producer_->send(e.key, e.payload); }
        catch (...) {  }
    }

    std::uint64_t dropped() const { return producer_->dropped(); }

private:
    std::unique_ptr<KafkaProducer> producer_;
};