#include <librdkafka/rdkafka.h>
#include <csignal>
#include <iostream>
#include <string>
#include <array>
#include <unistd.h>

static volatile std::sig_atomic_t running = 1;
static void on_sigint(int) { running = 0; }

int main(int argc, char* argv[]) {
    std::string brokers = "localhost:9092", topic = "chat.events";
    std::string group = "chat-tail-" + std::to_string(::getpid());
    bool from_beginning = true;

    for (int i = 1; i < argc; ++i) {
        std::string a(argv[i]);
        if (a.rfind("--brokers=", 0) == 0) brokers = a.substr(10);
        else if (a.rfind("--topic=", 0) == 0) topic = a.substr(8);
        else if (a.rfind("--group=", 0) == 0) group = a.substr(8);
        else if (a == "--live") from_beginning = false;
    }

    std::array<char, 512> err{};
    rd_kafka_conf_t* conf = rd_kafka_conf_new();
    rd_kafka_conf_set(conf, "bootstrap.servers", brokers.c_str(), err.data(), err.size());
    rd_kafka_conf_set(conf, "group.id", group.c_str(), err.data(), err.size());
    rd_kafka_conf_set(conf, "auto.offset.reset", from_beginning ? "earliest" : "latest", err.data(), err.size());
    rd_kafka_conf_set(conf, "enable.auto.commit", "true", err.data(), err.size());

    rd_kafka_t* rk = rd_kafka_new(RD_KAFKA_CONSUMER, conf, err.data(), err.size());
    if (!rk) { std::cerr << "consumer: " << err.data() << "\n"; return 1; }
    rd_kafka_poll_set_consumer(rk);

    auto* list = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(list, topic.c_str(), RD_KAFKA_PARTITION_UA);
    if (rd_kafka_subscribe(rk, list) != RD_KAFKA_RESP_ERR_NO_ERROR) {
        std::cerr << "subscribe failed\n";
        rd_kafka_topic_partition_list_destroy(list);
        rd_kafka_destroy(rk);
        return 1;
    }
    rd_kafka_topic_partition_list_destroy(list);

    std::signal(SIGINT, on_sigint);
    std::cout << "chat_tail: brokers=" << brokers << " topic=" << topic
              << " group=" << group << " (Ctrl+C для выхода)\n";

    while (running) {
        rd_kafka_message_t* m = rd_kafka_consumer_poll(rk, 500);
        if (!m) continue;
        if (m->err) {
            if (m->err != RD_KAFKA_RESP_ERR__PARTITION_EOF)
                std::cerr << "[err] " << rd_kafka_message_errstr(m) << "\n";
        } else {
            std::string key(static_cast<const char*>(m->key ? m->key : ""), m->key_len);
            std::string val(static_cast<const char*>(m->payload ? m->payload : ""), m->len);
            std::cout << "[p" << m->partition << " @" << m->offset << "] "
                      << "room=" << key << "  " << val << "\n" << std::flush;
        }
        rd_kafka_message_destroy(m);
    }

    std::cout << "\nchat_tail: завершение\n";
    rd_kafka_consumer_close(rk);
    rd_kafka_destroy(rk);
    return 0;
}