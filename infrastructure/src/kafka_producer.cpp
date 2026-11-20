#include "kafka_producer.hpp"
#include <stdexcept>
#include <array>

KafkaProducer::KafkaProducer(const std::string& brokers, const std::string& topic) {
    std::array<char, 512> err{};
    rd_kafka_conf_t* conf = rd_kafka_conf_new();

    auto set = [&](const char* k, const char* v) {
        if (rd_kafka_conf_set(conf, k, v, err.data(), err.size()) != RD_KAFKA_CONF_OK) {
            rd_kafka_conf_destroy(conf);
            throw std::runtime_error(std::string("kafka conf: ") + err.data());
        }
    };
    set("bootstrap.servers", brokers.c_str());
    set("acks", "1");
    set("linger.ms", "5");
    set("message.timeout.ms", "5000");
    set("socket.timeout.ms", "3000");
    set("queue.buffering.max.messages", "10000");  
    set("queue.buffering.max.kbytes", "16384");

    rk_ = rd_kafka_new(RD_KAFKA_PRODUCER, conf, err.data(), err.size());
    if (!rk_) { rd_kafka_conf_destroy(conf); throw std::runtime_error(std::string("kafka new: ") + err.data()); }

    rkt_ = rd_kafka_topic_new(rk_, topic.c_str(), nullptr);
    if (!rkt_) { rd_kafka_destroy(rk_); rk_ = nullptr; throw std::runtime_error("kafka topic_new failed"); }
}

KafkaProducer::~KafkaProducer() {
    if (rk_) flush(2000);
    if (rkt_) rd_kafka_topic_destroy(rkt_);
    if (rk_)  rd_kafka_destroy(rk_);
}

bool KafkaProducer::send(const std::string& key, const std::string& payload) noexcept {
    if (!rk_ || !rkt_) { ++dropped_; return false; }

    int rc = rd_kafka_produce(rkt_, RD_KAFKA_PARTITION_UA, RD_KAFKA_MSG_F_COPY,
                              const_cast<char*>(payload.data()), payload.size(),
                              key.data(), key.size(), nullptr);
    if (rc == -1) { ++dropped_; }
    else { ++sent_; }

    rd_kafka_poll(rk_, 0);  
    return rc != -1;
}

void KafkaProducer::flush(int timeout_ms) noexcept {
    if (rk_) rd_kafka_flush(rk_, timeout_ms);
}