#pragma once
#include "chat_app.hpp"
#include "ports/clock.hpp"
#include "ports/log_sink.hpp"
#include "ports/presenter.hpp"
#include "message_factory.hpp"
#include <memory>
#include <string_view>

struct AppComponents {
    std::unique_ptr<IClock> clock;
    std::unique_ptr<ILogSink> sink;
    std::unique_ptr<IPresenter> presenter;
    std::unique_ptr<MessageFactory> factory;
    std::unique_ptr<IChatStore> engine;
    std::unique_ptr<ChatApp> app;
};

AppComponents build_app(std::string_view engine_kind, std::string_view log_path);