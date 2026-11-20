#pragma once
#include "ports/clock.hpp"
#include "message.hpp"
#include <cstdint>
#include <string>
#include <utility>

// Pure Fabrication: штамповка ID и отметок времени не принадлежит
// ни Message (значение), ни IChatStore (хранилище).
class MessageFactory {
public:
    explicit MessageFactory(const IClock& clock) : clock_(clock) {}

    Message text(std::string author, std::string body) {
        return make(std::move(author), std::make_unique<TextBody>(std::move(body)));
    }
    Message system(std::string body) {
        return make("system", std::make_unique<SystemBody>(std::move(body)));
    }
    Message reaction(std::string author, std::string emoji) {
        return make(std::move(author), std::make_unique<ReactionBody>(std::move(emoji)));
    }
private:
    Message make(std::string author, std::unique_ptr<IMessageBody> body) {
        MessageHeader h{std::move(author), MessageId{++seq_, clock_.now()}};
        return Message(std::move(h), std::move(body));
    }
    const IClock& clock_;
    std::uint64_t seq_ = 0;
};