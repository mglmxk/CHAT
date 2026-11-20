#pragma once
#include "chat_app.hpp"
#include "ports/clock.hpp"
#include "ports/log_sink.hpp"
#include "ports/presenter.hpp"
#include "ports/event_publisher.hpp"
#include "message_factory.hpp"
#include <memory>
#include <string>

struct AppOptions {
    std::string engine  = "std";       
    std::string log_path;              
    std::string events  = "kafka";     
    std::string brokers = "localhost:9092";
    std::string topic   = "chat.events";
};

struct AppComponents {
    std::unique_ptr<IClock> clock;
    std::unique_ptr<ILogSink> sink;
    std::unique_ptr<IPresenter> presenter;
    std::unique_ptr<IEventPublisher> publisher;
    std::unique_ptr<MessageFactory> factory;
    std::unique_ptr<IChatStore> engine;
    std::unique_ptr<ChatApp> app;
    std::string events_mode;
};

AppComponents build_app(const AppOptions& opt);