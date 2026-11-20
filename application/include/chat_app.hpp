#pragma once
#include "ichat_store.hpp"
#include "message_factory.hpp"
#include "ports/presenter.hpp"
#include "ports/event_publisher.hpp"
#include "ports/clock.hpp"
#include "command_registry.hpp"
#include "command_catalog.hpp"
#include <string>
#include <memory>

class ChatApp {
public:
    ChatApp(IChatStore& store, MessageFactory& factory, IPresenter& ui,
            IEventPublisher& events, IClock& clock);
    void execute(const std::string& line);

private:
    CommandRegistry registry_;
    std::unique_ptr<CommandCatalog> catalog_;
    IPresenter* ui_ = nullptr;
};