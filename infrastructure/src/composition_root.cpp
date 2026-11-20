#include "composition_root.hpp"
#include "adapters/system_clock.hpp"
#include "adapters/file_log_sink.hpp"
#include "adapters/null_log_sink.hpp"
#include "adapters/console_presenter.hpp"
#include "adapters/vector_event_publisher.hpp"
#include "adapters/null_event_publisher.hpp"
#include "adapters/kafka_event_publisher.hpp"
#include "std_engine.hpp"
#include "hash_engine.hpp"
#include <iostream>

AppComponents build_app(const AppOptions& opt) {
    AppComponents c;
    c.events_mode = opt.events;
    c.clock = std::make_unique<SystemClock>();
    c.sink = opt.log_path.empty()
           ? std::unique_ptr<ILogSink>(std::make_unique<NullLogSink>())
           : std::unique_ptr<ILogSink>(std::make_unique<FileLogSink>(opt.log_path));
    c.presenter = std::make_unique<ConsolePresenter>();

    if (opt.events == "kafka") {
        try {
            c.publisher = std::make_unique<KafkaEventPublisher>(opt.brokers, opt.topic);
        } catch (const std::exception& e) {
            std::cerr << "[warn] события отключены: " << e.what() << "\n";
            c.publisher = std::make_unique<NullEventPublisher>();
            c.events_mode = "off";
        }
    } else if (opt.events == "memory") {
        c.publisher = std::make_unique<VectorEventPublisher>();
    } else {
        c.publisher = std::make_unique<NullEventPublisher>();
        c.events_mode = "off";
    }

    c.factory = std::make_unique<MessageFactory>(*c.clock);
    c.engine = (opt.engine == "hash")
             ? std::unique_ptr<IChatStore>(std::make_unique<HashEngine>(*c.sink))
             : std::unique_ptr<IChatStore>(std::make_unique<StdEngine>(*c.sink));
    c.app = std::make_unique<ChatApp>(*c.engine, *c.factory, *c.presenter, *c.publisher, *c.clock);
    return c;
}