#pragma once
#include <librdkafka/rdkafka.h>
#include <string>
#include <cstddef>
#include <cstdint>


class KafkaProducer {
public:
    KafkaProducer(const std::string& brokers, const std::string& topic);
    ~KafkaProducer();

    KafkaProducer(const KafkaProducer&) = delete;
    KafkaProducer& operator=(const KafkaProducer&) = delete;

    bool send(const std::string& key, const std::string& payload) noexcept;
    void flush(int timeout_ms) noexcept;

    std::uint64_t dropped() const noexcept { return dropped_; }
    std::uint64_t sent()    const noexcept { return sent_; }

private:
    rd_kafka_t* rk_ = nullptr;
    rd_kafka_topic_t* rkt_ = nullptr;
    std::uint64_t dropped_ = 0, sent_ = 0;
};